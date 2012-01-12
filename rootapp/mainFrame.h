#include <TGFrame.h> 
#include <TRootEmbeddedCanvas.h> 
#include <TCanvas.h>
#include <TThread.h>
#include <TGMenu.h>
#include <TGToolTip.h>
#include <RQ_OBJECT.h> 
#include <TH1.h>
#include <TH2.h>

class mainFrame : public TGMainFrame { 
  RQ_OBJECT("mainFrame") 
    ClassDef(mainFrame,1);

  private: 
    TGPopupMenu         *fMenuEdit;
    TGToolTip           *fToolTip;

    TRootEmbeddedCanvas *fEcanvas[20]; 
    TRootEmbeddedCanvas *fEcanvasrate[20]; 
    TRootEmbeddedCanvas *fEcanvasnhit; 
    TRootEmbeddedCanvas *fEcanvasnhitrate; 
    TCanvas *fCanvas[20];  
    TCanvas *fCanvasrate[20];  
    TCanvas *fCanvasnhit;  
    TCanvas *fCanvasnhitrate;
    TH2F *f1[20];
    TH2F *f1rate[20];
    TH1F *f1nhit;
    TH1F *f1nhitrate;
    double starttime[20];
    double endtime[20];
    double nhittime;
    Int_t nhithits;
    Int_t hits[10500];
    TThread             *thread;
    TThread             *AvalancheThread;
    Bool_t finished;
    Bool_t paused;
    int lastkey;
    int currentTab;

    enum menu_identifiers {
      M_FILE_OPEN,
      M_FILE_DRAW_PLOT,
      M_FILE_EXIT,
      M_EDIT_CLEAR_ALL,
      M_EDIT_PAUSE,
      M_EDIT_START
    };

  public: 
    mainFrame(const TGWindow *p,UInt_t w,UInt_t h); 
    virtual ~mainFrame(); 
    void DoDraw(); 
    void *thread_draw(void* arg);
    void *thread_avalanche(void* arg);
    void CloseWindow();
    void HandleMenu(Int_t id);
    void DoTab(Int_t id);
    void EventInfo(Int_t event, Int_t px, Int_t py, TObject *selected);
    virtual Int_t DistancetoPrimitive(Int_t px, Int_t py);
};

