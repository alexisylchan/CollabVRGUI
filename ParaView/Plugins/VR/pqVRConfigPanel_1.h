#include "pqLoadedFormObjectPanel.h"
#include "pqObjectPanelInterface.h"

class pqVRConfigPanel : public pqLoadedFormObjectPanel
{
  typedef pqObjectPanel Superclass;

  Q_OBJECT 
  public:
  
  pqVRConfigPanel (pqProxy* pxy, QWidget* p);
  ~pqVRConfigPanel();
  

};