/*=========================================================================

   Program: ParaView
   Module:    pqGlobalRenderViewOptions.h

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

#ifndef _pqGlobalRenderViewOptions_h
#define _pqGlobalRenderViewOptions_h

#include "pqComponentsExport.h"
#include "pqOptionsContainer.h"

/// options container for pages of render view options
class PQCOMPONENTS_EXPORT pqGlobalRenderViewOptions : public pqOptionsContainer
{
  Q_OBJECT

public:
  pqGlobalRenderViewOptions(QWidget *parent=0);
  virtual ~pqGlobalRenderViewOptions();

  // set the current page
  virtual void setPage(const QString &page);
  // return a list of strings for pages we have
  virtual QStringList getPageList();

  // apply the changes
  virtual void applyChanges();
  // reset the changes
  virtual void resetChanges();

  // tell pqOptionsDialog that we want an apply button
  virtual bool isApplyUsed() const { return true; }

protected:
  void init();

private slots:
  void lodThresholdSliderChanged(int);
  void lodResolutionSliderChanged(int);
  void outlineThresholdSliderChanged(int);
  void compositeThresholdSliderChanged(int);
  void subsamplingRateSliderChanged(int);
  void squirtColorspaceSliderChanged(int value);
  void zlibColorspaceSliderChanged(int value);
  void stillRenderSubsampleRateSliderChanged(int);
  void resetDefaultCameraManipulators();
  void applyCompressorDefaults();

private:
  class pqInternal;
  pqInternal* Internal;
};

#endif
