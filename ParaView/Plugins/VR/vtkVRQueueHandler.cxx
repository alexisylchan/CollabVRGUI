/*=========================================================================

   Program: ParaView
   Module:  vtkVRQueueHandler.cxx

   Copyright (c) 2005,2006 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2.

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

========================================================================*/
#include "vtkVRQueueHandler.h"

#include "vtkObjectFactory.h"
#include "vtkPVXMLElement.h"
#include "vtkPVXMLParser.h"
#include "vtkVRActiveObjectManipulationStyle.h"
#include "vtkVRInteractorStyle.h"
#include "vtkVRQueue.h"
#include "vtkVRStyleTracking.h"
#include "vtkVRStyleGrabNUpdateMatrix.h"
#include "vtkVRStyleGrabNRotateSliceNormal.h"
#include "vtkVRStyleGrabNTranslateSliceOrigin.h"
#include "vtkSMRenderViewProxy.h"
#include "pqApplicationCore.h"
#include "pqActiveObjects.h"
#include "pqView.h"

#include <QList>
#include <QPointer>
#include <QtDebug>
#include <QTimer>
#include <string>

class vtkVRQueueHandler::pqInternals
{
public:
  QList<QPointer<vtkVRInteractorStyle> > Styles;
  QPointer<vtkVRQueue> Queue;
  QTimer Timer;
};

//----------------------------------------------------------------------------
vtkVRQueueHandler::vtkVRQueueHandler(
  vtkVRQueue* queue, QObject* parentObject)
  : Superclass(parentObject)
{
  this->Internals = new pqInternals();
  this->Internals->Queue = queue;
  this->Internals->Timer.setInterval(1);
  this->Internals->Timer.setSingleShot(true);
  QObject::connect(&this->Internals->Timer, SIGNAL(timeout()),
    this, SLOT(processEvents()));

 /* QObject::connect(pqApplicationCore::instance(),
    SIGNAL(stateLoaded(vtkPVXMLElement*, vtkSMProxyLocator*)),
    this, SLOT(configureStyles(vtkPVXMLElement*, vtkSMProxyLocator*)));*/
  QObject::connect(pqApplicationCore::instance(),
    SIGNAL(stateSaved(vtkPVXMLElement*)),
    this, SLOT(saveStylesConfiguration(vtkPVXMLElement*)));
}

//----------------------------------------------------------------------------
vtkVRQueueHandler::~vtkVRQueueHandler()
{
  delete this->Internals;
}

//----------------------------------------------------------------------------
void vtkVRQueueHandler::add(vtkVRInteractorStyle* style)
{
  this->Internals->Styles.push_front(style);
}

//----------------------------------------------------------------------------
void vtkVRQueueHandler::remove(vtkVRInteractorStyle* style)
{
  this->Internals->Styles.removeAll(style);
}

//----------------------------------------------------------------------public
void vtkVRQueueHandler::clear()
{
  this->Internals->Styles.clear();
}

//----------------------------------------------------------------------------
void vtkVRQueueHandler::start()
{
  this->Internals->Timer.start();
  this->configureStyles();
}

//----------------------------------------------------------------------------
void vtkVRQueueHandler::stop()
{
  this->Internals->Timer.stop();
}

//----------------------------------------------------------------------------
void vtkVRQueueHandler::processEvents()
{
  Q_ASSERT(this->Internals->Queue != NULL);
  QQueue<vtkVREventData> events;
  this->Internals->Queue->tryDequeue(events);

  // Loop through the event queue and pass events to InteractorStyles
  while (!events.isEmpty())
    {
    vtkVREventData data = events.dequeue();
    foreach (vtkVRInteractorStyle* style, this->Internals->Styles)
      {
      if (style && style->handleEvent(data))
        {
        break;
        }
      }
    }

  // There should be an explicit update for each handler. Otherwise the server
  // side updates will not happen
  foreach (vtkVRInteractorStyle* style, this->Internals->Styles)
    {
    if (style && style->update())
      {
      break;
      }
    }

  this->render();

  // since timer is single-shot we start it again.
  this->Internals->Timer.start();
}

//----------------------------------------------------------------------------
void vtkVRQueueHandler::render()
{
  vtkSMRenderViewProxy *proxy =0;
  pqView *view = 0;
  view = pqActiveObjects::instance().activeView();
  if ( view )
    {
    proxy = vtkSMRenderViewProxy::SafeDownCast( view->getViewProxy() );
    if ( proxy )
      {
      proxy->StillRender();
      }
    }
}

//----------------------------------------------------------------------------
/* Sample configuration:
 <VRInteractorStyles>
    <Style class="vtkVRStyleGrabNRotateWorld">
      <Button name="wiimote.A"/>
      <Tracker name="wiimote.tracker"/>
    </Style>
    <Style class="vtkVRWandTrackingStyle">
      <Event name="wiitracker.hand" type = "tracker"/>
    </Style>
 </VRInteractorStyles>
 */
void vtkVRQueueHandler::configureStyles(vtkPVXMLElement* xml,
                                        vtkSMProxyLocator* locator)
{
  if (!xml)
    {
    return;
    }

  if (xml->GetName() && strcmp(xml->GetName(), "VRInteractorStyles") == 0)
    {
    this->clear();
    for (unsigned cc=0; cc < xml->GetNumberOfNestedElements(); cc++)
      {
      vtkPVXMLElement* child = xml->GetNestedElement(cc);
      if (child && child->GetName() && strcmp(child->GetName(), "Style")==0)
        {
        const char* class_name = child->GetAttributeOrEmpty("class");
        if (strcmp(class_name, "vtkVRStyleTracking")==0)
          {
          vtkVRStyleTracking* style = new vtkVRStyleTracking(this);
          style->configure(child, locator);
          this->add(style);
          }
        else if (strcmp(class_name, "vtkVRStyleGrabNUpdateMatrix")==0)
          {
          vtkVRStyleGrabNUpdateMatrix* style = new vtkVRStyleGrabNUpdateMatrix(this);
          style->configure(child, locator);
          this->add(style);
          }
        else if (strcmp(class_name, "vtkVRStyleGrabNTranslateSliceOrigin")==0)
          {
          vtkVRStyleGrabNTranslateSliceOrigin* style = new vtkVRStyleGrabNTranslateSliceOrigin(this);
          style->configure(child, locator);
          this->add(style);
          }
        else if (strcmp(class_name, "vtkVRStyleGrabNRotateSliceNormal")==0)
          {
          vtkVRStyleGrabNRotateSliceNormal* style = new vtkVRStyleGrabNRotateSliceNormal(this);
          style->configure(child, locator);
          this->add(style);
          }
        else if (strcmp(class_name, "vtkVRActiveObjectManipulationStyle")==0)
          {
          vtkVRActiveObjectManipulationStyle* style = new vtkVRActiveObjectManipulationStyle(this);
          style->configure(child, locator);
          this->add(style);
          }
        else
          {
          qWarning() << "Unknown interactor style: \"" << class_name << "\"";
          }
        }
      }
    }
  else
    {
    this->configureStyles(xml->FindNestedElementByName("VRInteractorStyles"),
                          locator);
    }
}


void vtkVRQueueHandler::configureStyles()
{
    const char* filename = "C:\\Users\\alexisc\\Documents\\EVE\\Collab\\plugin.pvsm";
	
	vtkPVXMLParser* parser = vtkPVXMLParser::New();
	parser->SetFileName(filename);
	parser->Parse();
	vtkPVXMLElement* xml = parser->GetRootElement();
	
	if (!pqApplicationCore::instance()->getActiveServer() || !xml)
	{
		return;
	}
  
	vtkSMProxyLocator* locator;// Placeholder, not actually used. TODO: change signature of style::configure
    if (xml->GetName() && strcmp(xml->GetName(), "VRInteractorStyles") == 0)
    {
    this->clear();
    for (unsigned cc=0; cc < xml->GetNumberOfNestedElements(); cc++)
      {
      vtkPVXMLElement* child = xml->GetNestedElement(cc);
      if (child && child->GetName() && strcmp(child->GetName(), "Style")==0)
        {
        const char* class_name = child->GetAttributeOrEmpty("class");
        if (strcmp(class_name, "vtkVRStyleTracking")==0)
          {
          vtkVRStyleTracking* style = new vtkVRStyleTracking(this);
          style->configure(child, locator);
          this->add(style);
          }
        else if (strcmp(class_name, "vtkVRStyleGrabNUpdateMatrix")==0)
          {
          vtkVRStyleGrabNUpdateMatrix* style = new vtkVRStyleGrabNUpdateMatrix(this);
          style->configure(child, locator);
          this->add(style);
          }
        else if (strcmp(class_name, "vtkVRStyleGrabNTranslateSliceOrigin")==0)
          {
          vtkVRStyleGrabNTranslateSliceOrigin* style = new vtkVRStyleGrabNTranslateSliceOrigin(this);
          style->configure(child, locator);
          this->add(style);
          }
        else if (strcmp(class_name, "vtkVRStyleGrabNRotateSliceNormal")==0)
          {
          vtkVRStyleGrabNRotateSliceNormal* style = new vtkVRStyleGrabNRotateSliceNormal(this);
          style->configure(child, locator);
          this->add(style);
          }
        else if (strcmp(class_name, "vtkVRActiveObjectManipulationStyle")==0)
          {
          vtkVRActiveObjectManipulationStyle* style = new vtkVRActiveObjectManipulationStyle(this);
          style->configure(child, locator);
          this->add(style);
          }
        else
          {
          qWarning() << "Unknown interactor style: \"" << class_name << "\"";
          }
        }
      }
    }
  else
    {
    this->configureStyles(xml->FindNestedElementByName("VRInteractorStyles"),
                          locator);
    }
}
 
void vtkVRQueueHandler::configureStyles(const char* connection_name, VRInteractorStyle interactor_style)
{ 
	vtkPVXMLElement* style_xml = vtkPVXMLElement::New();
	vtkSMProxyLocator* locator;
	vtkVRInteractorStyle* style;
	style_xml->SetName("Style");
	std::string concat_name;
	vtkPVXMLElement* tracker_xml;
	vtkPVXMLElement* property_xml;
	// TODO: Handle multiple trackers, allow user to specify tracker name (?)
	switch(interactor_style)
	{
	case HEAD_TRACKER_STYLE:
		style_xml->AddAttribute("class","vtkVRStyleTracking");
		style_xml->AddAttribute("set_property","RenderView1.HeadPose");

		tracker_xml = vtkPVXMLElement::New();
		tracker_xml->SetName("Tracker");
		concat_name = std::string(connection_name);
		concat_name = concat_name + "." + "HeadTracker";
		tracker_xml->AddAttribute("name", concat_name.c_str());
		style_xml->AddNestedElement(tracker_xml); 

		style = new vtkVRStyleTracking(this);
        style->configure(style_xml, locator);
        this->add(style);
		break;

	case PHANTOM_TRACKER_STYLE:
		style_xml->AddAttribute("class","vtkVRStyleGrabNUpdateMatrix");
		style_xml->AddAttribute("set_property","RenderView1.WandPose");

		tracker_xml = vtkPVXMLElement::New();
		tracker_xml->SetName("Tracker");
		concat_name = std::string(connection_name);
		concat_name = concat_name + "." + "PhantomTracker";
		tracker_xml->AddAttribute("name", concat_name.c_str());
		style_xml->AddNestedElement(tracker_xml); 

		property_xml = vtkPVXMLElement::New();
		property_xml->SetName("MatrixProperty"); 
		property_xml->AddAttribute("name", "RenderView1.WandPose");
		style_xml->AddNestedElement(property_xml);  

		style = new vtkVRStyleGrabNUpdateMatrix(this);
        style->configure(style_xml, locator);
        this->add(style);

		break;
	case ACTIVE_OBJ_TRACKER_STYLE:
		style_xml->AddAttribute("class","vtkVRStyleGrabNUpdateMatrix");
		style_xml->AddAttribute("set_property","RenderView1.WandPose");

		tracker_xml = vtkPVXMLElement::New();
		tracker_xml->SetName("Tracker");
		concat_name = std::string(connection_name);
		concat_name = concat_name + "." + "SpaceNavigator";
		tracker_xml->AddAttribute("name", concat_name.c_str());
		style_xml->AddNestedElement(tracker_xml); 

		property_xml = vtkPVXMLElement::New();
		property_xml->SetName("MatrixProperty"); 
		property_xml->AddAttribute("name", "RenderView1.WandPose");
		style_xml->AddNestedElement(property_xml);  

		style = new vtkVRStyleGrabNUpdateMatrix(this);
        style->configure(style_xml, locator);
        this->add(style);
		
		break;
	default:
		break;
	};
   
}

//----------------------------------------------------------------------------
void vtkVRQueueHandler::saveStylesConfiguration(vtkPVXMLElement* root)
{
  Q_ASSERT(root != NULL);

  vtkPVXMLElement* tempParent = vtkPVXMLElement::New();
  tempParent->SetName("VRInteractorStyles");
  foreach (vtkVRInteractorStyle* style, this->Internals->Styles)
    {
    vtkPVXMLElement* child = style->saveConfiguration();
    if (child)
      {
      tempParent->AddNestedElement(child);
      child->Delete();
      }
    }
  root->AddNestedElement(tempParent);
  tempParent->Delete();
}
