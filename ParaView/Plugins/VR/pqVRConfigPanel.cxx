#include "pqVRConfigPanel.h"
#include "ui_pqVRConfigPanel.h"
#include <QLayout>
#include <QLabel>

class pqVRConfigPanel::pqInternals
	: public Ui::pqVRConfigPanel
{
public:
	pqInternals(){};
	~pqInternals(){};

};
pqVRConfigPanel::pqVRConfigPanel(QWidget* p) :
QDockWidget("VR Configuration", p)
{
	this->Internals = new pqInternals();
	this->Internals->setupUi(this);
	QObject::connect(this->Internals->create_connection_button,SIGNAL(pressed()),
		this, SLOT(onCreate()));
}

pqVRConfigPanel::~pqVRConfigPanel()
{
	delete this->Internals;
}

void pqVRConfigPanel::onCreate()
{
	QString connection_name = this->Internals->device_name_textedit->toPlainText();
	QString server_name = this->Internals->vrpn_server_textedit->toPlainText();
	int num_buttons = this->Internals->number_of_buttons_spinbox->value();
	int curr_style = this->Internals->interactor_style_combobox->currentIndex();
	emit this->connectionSignal(connection_name.toAscii().data(),server_name.toAscii().data(),num_buttons,curr_style);
	emit this->styleSignal(connection_name.toAscii().data(),curr_style);
}