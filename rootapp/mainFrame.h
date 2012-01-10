#include <TGFrame.h> 
#include <TRootEmbeddedCanvas.h> 
#include <TThread.h>
#include <RQ_OBJECT.h> 
#include <TH1.h>

#ifndef __CINT__
#include <curl/curl.h>
#else
class CURL;
#endif


class mainFrame { 
  RQ_OBJECT("mainFrame") 

  private: 
    TGMainFrame         *fMain; 
    TRootEmbeddedCanvas *fEcanvas; 
    TThread             *thread;
    TThread             *AvalancheThread;
    Bool_t finished;
    CURL *curl;
    int lastkey;
    TH1F *f1;

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
    void curl_write(char *ptr);

};

