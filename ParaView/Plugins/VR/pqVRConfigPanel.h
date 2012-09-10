#include <QDockWidget>

class pqVRConfigPanel : public QDockWidget
{
  Q_OBJECT 
  public:
  
  pqVRConfigPanel (QWidget* p=0);
  ~pqVRConfigPanel();	
 
  signals:
	  void styleSignal(const char* connection_name, int interactor_style);
	  void connectionSignal(const char* connection_name, const char* server_address,
								int num_buttons, int type_enum);


private slots:
	void onCreate();

private:
	class pqInternals;
	pqInternals *Internals;

};