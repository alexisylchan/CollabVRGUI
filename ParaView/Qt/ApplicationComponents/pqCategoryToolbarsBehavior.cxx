/*=========================================================================

   Program: ParaView
   Module:    pqCategoryToolbarsBehavior.cxx

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
#include "pqCategoryToolbarsBehavior.h"

#include "pqProxyGroupMenuManager.h"

#include <QMainWindow>
#include <QToolBar>

//-----------------------------------------------------------------------------
pqCategoryToolbarsBehavior::pqCategoryToolbarsBehavior(
  pqProxyGroupMenuManager* menuManager, QMainWindow* mainWindow)
  : Superclass(menuManager)
{
  Q_ASSERT(menuManager != 0);
  Q_ASSERT(mainWindow != 0);

  this->MainWindow = mainWindow;
  this->MenuManager = menuManager;

  QObject::connect(menuManager, SIGNAL(menuPopulated()),
    this, SLOT(updateToolbars()));
  this->updateToolbars();
}

//-----------------------------------------------------------------------------
void pqCategoryToolbarsBehavior::updateToolbars()
{
  QStringList toolbarCategories = this->MenuManager->getToolbarCategories();
  foreach (QString category, toolbarCategories)
    {
    QToolBar* toolbar = this->MainWindow->findChild<QToolBar*>(category);
    if (!toolbar)
      {
      this->MainWindow->addToolBarBreak();
      toolbar = new QToolBar(this->MainWindow);
      toolbar->setObjectName(category);
      toolbar->setOrientation(Qt::Horizontal);
      toolbar->setWindowTitle(category); // TODO: Get label from MenuManager.
      this->MainWindow->addToolBar(toolbar);
      }
    QList<QAction*> toolbarActions = this->MenuManager->actions(category);
    toolbar->clear();
    for (int cc=0; cc < toolbarActions.size(); cc++)
      {
      toolbar->addAction(toolbarActions[cc]);
      }
    }
}


