#ifndef ROOTAPP_H
#define ROOTAPP_H

#include "TGFrame.h" 
#include "TThread.h"
#include "TGMenu.h"
#include "TGToolTip.h"
#include "TGTab.h"
#include "RQ_OBJECT.h" 
#include <vector>

#include "Plot.h"

class RootApp : public TGMainFrame { 
  RQ_OBJECT("RootApp") 
    ClassDef(RootApp,1);

  private: 
    Bool_t              fFinished;
    
    TGMenuBar           *fMenuBar;
    TGPopupMenu         *fMenuFile;
    TGPopupMenu         *fMenuEdit;
    TGPopupMenu         *fMenuCrate;
    TGPopupMenu         *fMenuHelp;
    TGToolTip           *fToolTip;

    TGCompositeFrame    *fMainFrame;
    TGTab               *fDisplayFrame;
    TGCompositeFrame    *fTab1;
    TGCompositeFrame    *fTab2;
    TGCompositeFrame    *fTab3;
    TGCompositeFrame    *fTab4;
    TGCompositeFrame    *fTab5;
    TGCompositeFrame    *fTab6;

    TThread             *fDrawThread;
    TThread             *fDispatchThread;

    Int_t               fCurrentTab;
    Int_t               fCurrentCrate;

  public: 
    RootApp(const TGWindow *p,UInt_t w,UInt_t h); 
    virtual ~RootApp(); 

    void SetupMenus();
    void SetupTabs();

    Int_t GetCurrentCrate(){return fCurrentCrate;};
    Int_t GetCurrentTab(){return fCurrentTab;};
    Bool_t IsFinished(){return fFinished;};
    virtual void CloseWindow();
    void HandleMenu(Int_t id);
    void EventInfo(Int_t event, Int_t px, Int_t py, TObject *selected);

    void DoDraw(); 
    void DoTab(Int_t id);

    static void *DispatchThread(void* arg);
    static void *DrawThread(void* arg);

    //tab1
    HistPlot            *fNhit;
    TimeRatePlot        *fNhitTimeRate;
    //tab2
    Hist2dPlot          *fCCCHits[20];
    //tab3
    Rate2dPlot          *fCCCRate[20];        
    //tab4
    Rate2dPlot          *fCCCCmosRate[20];        
    //tab5
    HistPlot            *fCrateHits[20];
    RatePlot            *fCrateRate[20];
    RatePlot            *fCrateCmosRate[20];
    TimeRatePlot        *fCrateTimeRate[20];
    HistPlot            *fCrateNhit[20];
    //tab6
    HistPlot            *fTrigCount;
    RatePlot            *fTrigRate;

    std::vector<Plot*>  fPlots;

};

#endif
