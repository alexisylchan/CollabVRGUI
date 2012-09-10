/*=========================================================================

   Program: ParaView
   Module:    pqAutoStartImplementation.cxx.in

   Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
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

=========================================================================*/

#include "pqVRStarterImplementation.h"
#include "pqVRStarter.h"
#include "pqVRConfigPanel.h"

//-----------------------------------------------------------------------------
pqVRStarterImplementation::pqVRStarterImplementation(QObject* _parent)
  : QObject(_parent)
{
  this->UserImplementation = new pqVRStarter();
  this->MyDock = 0;
}

//-----------------------------------------------------------------------------
pqVRStarterImplementation::~pqVRStarterImplementation()
{
  delete this->UserImplementation;
  this->UserImplementation = 0;
}

//-----------------------------------------------------------------------------
void pqVRStarterImplementation::startup()
{
  this->UserImplementation->onStartup();
}

//-----------------------------------------------------------------------------
void pqVRStarterImplementation::shutdown()
{
  this->UserImplementation->onShutdown();
}
QString pqVRStarterImplementation::dockArea() const
{
  return "Left";
}

QDockWidget* pqVRStarterImplementation::dockWindow(QWidget* p)
{
  if(!this->MyDock)
    {
    this->MyDock = new pqVRConfigPanel(p);
    this->MyDock->setObjectName("pqVRConfigPanel");
	QObject::connect(this->MyDock, SIGNAL(connectionSignal(const char*, const char*,int, int)),
		this->UserImplementation,SLOT(configureVRPNConnection(const char*, const char*, int, int)));

	QObject::connect(this->MyDock, SIGNAL(styleSignal(const char*, int)),
		this->UserImplementation,SLOT(configureStyles(const char*, int)));

    }
  return this->MyDock;
}


