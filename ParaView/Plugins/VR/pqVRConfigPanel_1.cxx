#include "pqVRConfigPanel.h"

#include <QLayout>
#include <QLabel>
pqVRConfigPanel::pqVRConfigPanel(pqProxy* object_proxy, QWidget* p) :
pqLoadedFormObjectPanel(":/Resources/pqVRConfigPanel.ui",object_proxy, p)
{
	this->layout()->addWidget(new QLabel("This is from a plugin", this));
}

pqVRConfigPanel::~pqVRConfigPanel()
{

}
