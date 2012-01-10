#include <TGFrame.h> 
#include <TRootEmbeddedCanvas.h> 
#include <TCanvas.h>
#include <TThread.h>
#include <RQ_OBJECT.h> 
#include <TH1.h>
#include <TH2.h>

#ifndef __CINT__
#include <curl/curl.h>
#else
class CURL;
#endif


class mainFrame { 
  RQ_OBJECT("mainFrame") 

  private: 
    TGMainFrame         *fMain; 
    TRootEmbeddedCanvas *fEcanvas[20]; 
    TRootEmbeddedCanvas *fEcanvasrate[20]; 
    TRootEmbeddedCanvas *fEcanvasnhit; 
    TCanvas *fCanvas[20];  
    TCanvas *fCanvasrate[20];  
    TCanvas *fCanvasnhit;  
    TH2F *f1[20];
    TH2F *f1rate[20];
    TH1F *f1nhit;
    double starttime[20];
    double endtime[20];
    Int_t hits[10000];
    TThread             *thread;
    TThread             *AvalancheThread;
    Bool_t finished;
    CURL *curl;
    int lastkey;
    int currentTab;

    enum menu_identifiers {
      M_FILE_OPEN,
      M_FILE_DRAW_PLOT,
      M_FILE_EXIT
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
    void curl_write(char *ptr);

};

