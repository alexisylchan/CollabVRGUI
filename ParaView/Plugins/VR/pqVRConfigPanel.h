#include <QDockWidget>

class pqVRConfigPanel : public QDockWidget
{
  Q_OBJECT 
  public:
  
  pqVRConfigPanel (QWidget* p=0);
  ~pqVRConfigPanel();

private slots:
	void onCreate();

private:
	class pqInternals;
	pqInternals *Internals;

};