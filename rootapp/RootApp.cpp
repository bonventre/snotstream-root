#include "TApplication.h"
#include "TThread.h"
#include "TGFrame.h"
#include "TGObject.h"
#include "TGClient.h" 
#include "TCanvas.h"  
#include "TH1.h" 
#include "TH2.h" 
#include "TGButton.h" 
#include "TGMenu.h"
#include "TSystem.h"
#include "TApplication.h"
#include "TGTableLayout.h"
#include "TGTab.h"
#include "TGToolTip.h"
#include "time.h"
#include "stdlib.h"
#include "stdio.h"
#include "RAT/DS/PackedEvent.hh"
#include "avalanche.hpp"

#include "RootApp.h"

#define M_FILE_OPEN       0
#define M_FILE_DRAW_PLOT  1
#define M_FILE_EXIT       2
#define M_EDIT_CLEAR_ALL  3
#define M_EDIT_PAUSE      4
#define M_EDIT_START      5
#define M_CRATE_OFFSET    1000
#define M_CRATE_NUM(a)    M_CRATE_OFFSET+a
#define M_CARD_OFFSET    2000
#define M_CARD_NUM(a)    M_CARD_OFFSET+a

RootApp::RootApp(const TGWindow *p,UInt_t w,UInt_t h) : TGMainFrame(p,w,h) { 
  fFinished = kFALSE;
  fCurrentTab = 0;
  fDrawThread = 0;

  for (Int_t i=0;i<20;i++){
    char tempname[100];
    sprintf(tempname,"Crate %02d CC hits",i);
    fCCCHits[i] = new Hist2dPlot(this,tempname,"Hits per channel",16,0,16,32,0,32);
    fCCCHits[i]->SetXaxisLabel("Card");
    fCCCHits[i]->SetYaxisLabel("Channel");
    fPlots.push_back(fCCCHits[i]);

    sprintf(tempname,"Crate %02d CC rate",i);
    fCCCRate[i] = new Rate2dPlot(this,tempname,"Hit Rate (hits/s) per channel",16,0,16,32,0,32);
    fCCCRate[i]->SetXaxisLabel("Card");
    fCCCRate[i]->SetYaxisLabel("Channel");
    fPlots.push_back(fCCCRate[i]);

    sprintf(tempname,"Crate %02d CMOS rate",i);
    fCCCCmosRate[i] = new Rate2dPlot(this,tempname,"CMOS Rate (1/s) per channel",16,0,16,32,0,32);
    fCCCCmosRate[i]->SetXaxisLabel("Card");
    fCCCCmosRate[i]->SetYaxisLabel("Channel");
    fPlots.push_back(fCCCCmosRate[i]);

    sprintf(tempname,"Crate %02d hits",i);
    fCrateHits[i] = new HistPlot(this,tempname,"Hits per channel",512,0,512);
    fCrateHits[i]->SetXaxisLabel("Channel");
    fPlots.push_back(fCrateHits[i]);

    sprintf(tempname,"Crate %02d rate",i);
    fCrateRate[i]  = new RatePlot(this,tempname,"Hit Rate (hits/s) per channel",512,0,512);
    fCrateRate[i]->SetXaxisLabel("Channel");
    fPlots.push_back(fCrateRate[i]);

    sprintf(tempname,"Crate %02d CMOS rate",i);
    fCrateCmosRate[i]  = new RatePlot(this,tempname,"CMOS Rate (1/s) per channel",512,0,512);
    fCrateCmosRate[i]->SetXaxisLabel("Channel");
    fPlots.push_back(fCrateCmosRate[i]);

    sprintf(tempname,"Crate %02d scrolling rate",i);
    fCrateTimeRate[i] = new TimeRatePlot(this,tempname,"Scrolling average hit rate (hits/s)",20,-20,0);
    fCrateTimeRate[i]->SetXaxisLabel("Time (seconds)");
    fPlots.push_back(fCrateTimeRate[i]);
    
    sprintf(tempname,"Crate %02d NHit",i);
    fCrateNhit[i] = new HistPlot(this,tempname,"Nhit",20,0,40);
    fCrateNhit[i]->SetXaxisLabel("Nhit");
    fPlots.push_back(fCrateNhit[i]);
  }

  for (Int_t i=0;i<20;i++){
    for (Int_t j=0;j<16;j++){
    char tempname[100];
    sprintf(tempname,"Crate %d card %d hits",i,j);
    fCardHits[i*16+j] = new HistPlot(this,tempname,"Hits per channel",32,0,32);
    fCardHits[i*16+j]->SetXaxisLabel("Channel");
    fPlots.push_back(fCardHits[i*16+j]);
  
    sprintf(tempname,"Crate %d card %d rate",i,j);
    fCardRate[i*16+j] = new RatePlot(this,tempname,"Hit Rate (hits/s) per channel",32,0,32);
    fCardRate[i*16+j]->SetXaxisLabel("Channel");
    fPlots.push_back(fCardRate[i*16+j]);
    
    sprintf(tempname,"Crate %d card %d CMOS rate",i,j);
    fCardCmosRate[i*16+j] = new RatePlot(this,tempname,"CMOS rate (1/s) per channel",32,0,32);
    fCardCmosRate[i*16+j]->SetXaxisLabel("Channel");
    fPlots.push_back(fCardCmosRate[i*16+j]);
    
    sprintf(tempname,"Crate %d card %d scrolling rate",i,j);
    fCardTimeRate[i*16+j] = new TimeRatePlot(this,tempname,"Scrolling average hit rate (hits/s)",20,-20,0);
    fCardTimeRate[i*16+j]->SetXaxisLabel("Time (seconds)");
    fPlots.push_back(fCardTimeRate[i*16+j]);
    
    sprintf(tempname,"Crate %d card %d NHit",i,j);
    fCardNhit[i*16+j] = new HistPlot(this,tempname,"Nhit",20,0,40);
    fCardNhit[i*16+j]->SetXaxisLabel("Nhit");
    fPlots.push_back(fCardNhit[i*16+j]);
    }
  }

  fNhit = new HistPlot(this,"Nhit","Nhit",20,0,400);
  fNhit->SetXaxisLabel("Nhit");
  fPlots.push_back(fNhit);
  
  fNhitTimeRate = new TimeRatePlot(this,"Nhit rate","Scrolling average total hit rate (hits/s)",20,-20,0);
  fNhitTimeRate->SetXaxisLabel("Time (seconds)");
  fPlots.push_back(fNhitTimeRate);
  
  char triggernames[9][30] = {"N100L","N100M","N100H","N20","N20L","ESUML","ESUMH","PulseGT","PreScale"};
  fTrigCount = new HistPlot(this,"Trigger Counts","Trigger Counts",9,0,9);
  fTrigCount->SetBinLabels(triggernames);
  fPlots.push_back(fTrigCount);
  
  fTrigRate  = new RatePlot(this,"Trigger Rates","Trigger Rates",9,0,9);
  fTrigRate->SetBinLabels(triggernames);
  fPlots.push_back(fTrigRate);

  fDispatchThread = new TThread("dispatch",(void* (*) (void *)) &RootApp::DispatchThread,(void*) this);
  fDispatchThread->Run();

  fToolTip = new TGToolTip(fClient->GetDefaultRoot(),fMainFrame,"",250);

  fMainFrame = new TGVerticalFrame(this,100,100);
  AddFrame(fMainFrame, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY,0,0,0,0));  
  SetupMenus();
  SetupTabs(); 


  // Set a name to the main frame 
  SetWindowName("SNOTSTREAM"); 
  // Map all subwindows of main frame 
  MapSubwindows(); 
  // Initialize the layout algorithm 
  Resize(this->GetDefaultSize()); 
  // Map main frame 
  MapWindow(); 

  while (!fFinished) {
    for (Int_t i=0;i<fPlots.size();i++){
      fPlots[i]->Update();
    }

    gSystem->ProcessEvents();
    usleep(100);
  }

  if (fDispatchThread)
    fDispatchThread->Join();
  if (fDrawThread)
    fDrawThread->Join();

  gApplication->Terminate();
}

RootApp::~RootApp() { 
  // Clean up used widgets: frames, buttons, layouthints 
  Cleanup(); 
  delete fMenuBar;
  delete fMenuFile;
  delete fMenuEdit;
  delete fMenuHelp;
  delete fMainFrame;
  delete fDisplayFrame;
  delete fTab1;
  delete fTab2;
  delete fTab3;
  delete fTab4;
  delete fTab5;
  delete fTab6;
  delete fTab7;
  for (Int_t i=0;i<fPlots.size();i++){
    delete fPlots[i];
  }
  delete fDispatchThread;
  delete fDrawThread;

}

void RootApp::SetupMenus()
{
  fMenuBar = new TGMenuBar(this,100,20,kHorizontalFrame);

  fMenuFile = new TGPopupMenu(gClient->GetRoot());
  fMenuFile->AddEntry("&Open",M_FILE_OPEN);
  fMenuFile->AddEntry("&Draw Plot",M_FILE_DRAW_PLOT);
  fMenuFile->AddSeparator();
  fMenuFile->AddEntry("E&xit",M_FILE_EXIT);
  fMenuFile->Connect("Activated(Int_t)","RootApp",this,"HandleMenu(Int_t)");
  fMenuBar->AddPopup("&File",fMenuFile,new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0) );

  fMenuEdit = new TGPopupMenu(gClient->GetRoot());
  fMenuEdit->AddEntry("C&lear All Plots",M_EDIT_CLEAR_ALL);
  fMenuEdit->AddSeparator();
  fMenuEdit->AddEntry("&Pause",M_EDIT_PAUSE);
  fMenuEdit->AddEntry("&Start",M_EDIT_START);
  fMenuEdit->DisableEntry(M_EDIT_PAUSE);
  fMenuEdit->DisableEntry(M_EDIT_START);
  fMenuEdit->Connect("Activated(Int_t)","RootApp",this,"HandleMenu(Int_t)");
  fMenuBar->AddPopup("&Edit",fMenuEdit,new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0) );

  fMenuCrate = new TGPopupMenu(gClient->GetRoot());
  for (Int_t i=0;i<20;i++){
    char tempname[40];
    sprintf(tempname,"Crate &%d\n",i);
    fMenuCrate->AddEntry(tempname,M_CRATE_NUM(i));
  }
  fMenuCrate->CheckEntry(M_CRATE_NUM(0));
  fCurrentCrate = 0;
  fMenuCrate->Connect("Activated(Int_t)","RootApp",this,"HandleMenu(Int_t)");
  fMenuBar->AddPopup("&Crate",fMenuCrate,new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0) );

  fMenuCard = new TGPopupMenu(gClient->GetRoot());
  for (Int_t i=0;i<16;i++){
    char tempname[40];
    sprintf(tempname,"Card &%d\n",i);
    fMenuCard->AddEntry(tempname,M_CARD_NUM(i));
  }
  fMenuCard->CheckEntry(M_CARD_NUM(0));
  fCurrentCard = 0;
  fMenuCard->Connect("Activated(Int_t)","RootApp",this,"HandleMenu(Int_t)");
  fMenuBar->AddPopup("Car&d",fMenuCard,new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0) );



  fMenuHelp = new TGPopupMenu(gClient->GetRoot());
  fMenuHelp->AddEntry("&About",1);
  fMenuBar->AddPopup("&Help",fMenuHelp,new TGLayoutHints(kLHintsTop | kLHintsRight) );

  fMainFrame->AddFrame(fMenuBar,new TGLayoutHints(kLHintsTop | kLHintsExpandX));
}

void RootApp::SetupTabs()
{
  fDisplayFrame = new TGTab(this,100,100);
  fDisplayFrame->Connect("Selected(Int_t)", "RootApp",this,"DoTab(Int_t)");

  fTab1 = fDisplayFrame->AddTab("NHit");
  fTab1->SetLayoutManager(new TGTableLayout(fTab1,2,1));
  fNhit->CreateECanvas("nhit",fTab1,100,100); 
  fTab1->AddFrame(fNhit->GetECanvas(),new TGTableLayoutHints(0,1,0,1,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
  fNhitTimeRate->CreateECanvas("nhit rate",fTab1,100,100); 
  fTab1->AddFrame(fNhitTimeRate->GetECanvas(),new TGTableLayoutHints(0,1,1,2,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));

  fTab2 = fDisplayFrame->AddTab("Hits per Channel");
  fTab2->SetLayoutManager(new TGTableLayout(fTab2,5,4));
  for (Int_t i=0;i<5;i++){
    for (Int_t j=0;j<4;j++){
      char tempname[10];
      sprintf(tempname,"%02d",i*4+j);
      fCCCHits[i*4+j]->CreateECanvas(tempname,fTab2,100,100); 
      fTab2->AddFrame(fCCCHits[i*4+j]->GetECanvas(),new TGTableLayoutHints(j,j+1,i,i+1,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
    }
  }

  fTab3 = fDisplayFrame->AddTab("Hit Rate per Channel");
  fTab3->SetLayoutManager(new TGTableLayout(fTab3,5,4));
  for (Int_t i=0;i<5;i++){
    for (Int_t j=0;j<4;j++){
      char tempname[10];
      sprintf(tempname,"%02d rate",i*4+j);
      fCCCRate[i*4+j]->CreateECanvas(tempname,fTab3,100,100); 
      fTab3->AddFrame(fCCCRate[i*4+j]->GetECanvas(),new TGTableLayoutHints(j,j+1,i,i+1,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
    }
  }

  fTab4 = fDisplayFrame->AddTab("Cmos Rate per Channel");
  fTab4->SetLayoutManager(new TGTableLayout(fTab4,5,4));
  for (Int_t i=0;i<5;i++){
    for (Int_t j=0;j<4;j++){
      char tempname[20];
      sprintf(tempname,"%02d cmos rate",i*4+j);
      fCCCCmosRate[i*4+j]->CreateECanvas(tempname,fTab4,100,100);
      fTab4->AddFrame(fCCCCmosRate[i*4+j]->GetECanvas(),new TGTableLayoutHints(j,j+1,i,i+1,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
    }
  }

  fTab5 = fDisplayFrame->AddTab("Crate");
  fTab5->SetLayoutManager(new TGTableLayout(fTab5,3,2));
  fCrateHits[0]->CreateECanvas("hits",fTab5,100,100);
  fTab5->AddFrame(fCrateHits[0]->GetECanvas(),new TGTableLayoutHints(0,1,0,1,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
  fCrateTimeRate[0]->CreateECanvas("time rate",fTab5,100,100);
  fTab5->AddFrame(fCrateTimeRate[0]->GetECanvas(),new TGTableLayoutHints(1,2,0,1,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
  fCrateRate[0]->CreateECanvas("rate",fTab5,100,100);
  fTab5->AddFrame(fCrateRate[0]->GetECanvas(),new TGTableLayoutHints(0,1,1,2,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
  fCrateCmosRate[0]->CreateECanvas("cmos rate",fTab5,100,100);
  fTab5->AddFrame(fCrateCmosRate[0]->GetECanvas(),new TGTableLayoutHints(1,2,1,2,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
  fCrateNhit[0]->CreateECanvas("nhit",fTab5,100,100);
  fTab5->AddFrame(fCrateNhit[0]->GetECanvas(),new TGTableLayoutHints(0,2,2,3,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
  for (Int_t i=1;i<20;i++){
    fCrateHits[i]->SetECanvas(fCrateHits[0]->GetECanvas());
    fCrateTimeRate[i]->SetECanvas(fCrateTimeRate[0]->GetECanvas());
    fCrateNhit[i]->SetECanvas(fCrateNhit[0]->GetECanvas());
    fCrateRate[i]->SetECanvas(fCrateRate[0]->GetECanvas());
    fCrateCmosRate[i]->SetECanvas(fCrateCmosRate[0]->GetECanvas());
  }

  fTab6 = fDisplayFrame->AddTab("Card");
  fTab6->SetLayoutManager(new TGTableLayout(fTab6,3,2));
  fCardHits[0]->CreateECanvas("hits",fTab6,100,100);
  fTab6->AddFrame(fCardHits[0]->GetECanvas(),new TGTableLayoutHints(0,1,0,1,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
  fCardTimeRate[0]->CreateECanvas("time rate",fTab6,100,100);
  fTab6->AddFrame(fCardTimeRate[0]->GetECanvas(),new TGTableLayoutHints(1,2,0,1,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
  fCardRate[0]->CreateECanvas("rate",fTab6,100,100);
  fTab6->AddFrame(fCardRate[0]->GetECanvas(),new TGTableLayoutHints(0,1,1,2,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
  fCardCmosRate[0]->CreateECanvas("cmos rate",fTab6,100,100);
  fTab6->AddFrame(fCardCmosRate[0]->GetECanvas(),new TGTableLayoutHints(1,2,1,2,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
  fCardNhit[0]->CreateECanvas("nhit",fTab6,100,100);
  fTab6->AddFrame(fCardNhit[0]->GetECanvas(),new TGTableLayoutHints(0,2,2,3,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
  for (Int_t i=1;i<320;i++){
    fCardHits[i]->SetECanvas(fCardHits[0]->GetECanvas());
    fCardTimeRate[i]->SetECanvas(fCardTimeRate[0]->GetECanvas());
    fCardNhit[i]->SetECanvas(fCardNhit[0]->GetECanvas());
    fCardRate[i]->SetECanvas(fCardRate[0]->GetECanvas());
    fCardCmosRate[i]->SetECanvas(fCardCmosRate[0]->GetECanvas());
  }

  fTab7 = fDisplayFrame->AddTab("Triggers");
  fTab7->SetLayoutManager(new TGTableLayout(fTab7,2,1));
  fTrigCount->CreateECanvas("trigcounts",fTab7,100,100);
  fTab7->AddFrame(fTrigCount->GetECanvas(),new TGTableLayoutHints(0,1,0,1,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
  fTrigRate->CreateECanvas("trigcounts",fTab7,100,100);
  fTab7->AddFrame(fTrigRate->GetECanvas(),new TGTableLayoutHints(0,1,1,2,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));

  fMainFrame->AddFrame(fDisplayFrame, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY, 
        10,10,10,1));  
}

void RootApp::CloseWindow()
{
  fFinished = kTRUE;
}

void RootApp::DoTab(Int_t id)
{
  fCurrentTab = id;
}

void RootApp::HandleMenu(Int_t id)
{
  switch (id){
    case M_FILE_OPEN:
      printf("cant do that yet!\n");
      break;
    case M_FILE_DRAW_PLOT:
      DoDraw();
      break;
    case M_FILE_EXIT:
      CloseWindow();
      break;
    case M_EDIT_CLEAR_ALL:
      for (Int_t i=0;i<fPlots.size();i++){
        fPlots[i]->Clear();
      }
      break;
    case M_EDIT_PAUSE:
      for (Int_t i=0;i<fPlots.size();i++){
        fPlots[i]->Pause();
      }
      fMenuEdit->DisableEntry(M_EDIT_PAUSE);
      fMenuEdit->EnableEntry(M_EDIT_START);
      break;
    case M_EDIT_START:
      for (Int_t i=0;i<fPlots.size();i++){
        fPlots[i]->UnPause();
      }
      fMenuEdit->DisableEntry(M_EDIT_START);
      fMenuEdit->EnableEntry(M_EDIT_PAUSE);
      break;
  }
  if (id >= M_CARD_OFFSET){
    for (Int_t i=0;i<20;i++)
      fMenuCard->UnCheckEntry(M_CARD_NUM(i));
    fMenuCard->CheckEntry(id);
    Int_t card = (id-M_CARD_OFFSET) + fCurrentCrate*16;
    fCurrentCard = card;
    TThread::Lock();
    fCardHits[card]->Draw();
    fCardRate[card]->Draw();
    fCardTimeRate[card]->Draw();
    fCardCmosRate[card]->Draw();
    fCardNhit[card]->Draw();
    TThread::UnLock();
    fDisplayFrame->SetTab(5);
  }else
  if (id >= M_CRATE_OFFSET){
    for (Int_t i=0;i<20;i++)
      fMenuCrate->UnCheckEntry(M_CRATE_NUM(i));
    fMenuCrate->CheckEntry(id);
    Int_t crate = id-M_CRATE_OFFSET;
    fCurrentCard = fCurrentCard-fCurrentCrate*16+crate*16;
    fCurrentCrate = crate;

    TThread::Lock();
    fCrateHits[crate]->Draw();
    fCrateRate[crate]->Draw();
    fCrateCmosRate[crate]->Draw();
    fCrateTimeRate[crate]->Draw();
    fCrateNhit[crate]->Draw();
    
    fCardHits[fCurrentCard]->Draw();
    fCardRate[fCurrentCard]->Draw();
    fCardTimeRate[fCurrentCard]->Draw();
    fCardCmosRate[fCurrentCard]->Draw();
    fCardNhit[fCurrentCard]->Draw();
    TThread::UnLock();

    fDisplayFrame->SetTab(4);
  }
}

void RootApp::EventInfo(Int_t event, Int_t px, Int_t py, TObject *selected)
{
  fToolTip->Hide();
  if (selected == 0 || event != kMouseMotion || (strcmp(selected->ClassName(),"TH1F") != 0 && strcmp(selected->ClassName(),"TH2F") != 0))
    return;
  TString tipInfo;
  TString objInfo = selected->GetObjectInfo(px, py);
  if (objInfo.BeginsWith("-")) {
    // if the string begins with '-', display only the object info
    objInfo.Remove(TString::kLeading, '-');
    tipInfo = objInfo;
  }
  else {
    const char *title = selected->GetTitle();
    tipInfo += TString::Format("%s::%s", selected->ClassName(),
        selected->GetName());
    if (title && strlen(title))
      tipInfo += TString::Format("\n%s", selected->GetTitle());
    tipInfo += TString::Format("\n%d, %d", px, py);
    if (!objInfo.IsNull())
      tipInfo += TString::Format("\n%s", objInfo.Data());
  }
  fToolTip->SetText(tipInfo.Data());
  fToolTip->SetPosition(px+15, py+15);
  fToolTip->Reset();
}


void RootApp::DoDraw() { 
  if (!fDrawThread){
    fMenuEdit->EnableEntry(M_EDIT_PAUSE);
    fDrawThread = new TThread("memberfunction",(void *(*) (void *)) &RootApp::DrawThread,(void *)this);
    fDrawThread->Run();
  }
}

void *RootApp::DrawThread(void* arg)
{
  RootApp* temp = (RootApp*) arg;
  int upd = 5000;

  TThread::Lock();
  for (Int_t i=0;i<20;i++){
    temp->fCCCHits[i]->Draw("LEGO");
    temp->fCCCRate[i]->Draw("LEGO");
    temp->fCCCCmosRate[i]->Draw("LEGO");
  }
  temp->fNhit->Draw();
  temp->fNhitTimeRate->Draw();
  temp->fCrateHits[0]->Draw();
  temp->fCrateRate[0]->Draw();
  temp->fCrateCmosRate[0]->Draw();
  temp->fCrateTimeRate[0]->Draw();
  temp->fCrateNhit[0]->Draw();
  temp->fCardHits[0]->Draw();
  temp->fCardRate[0]->Draw();
  temp->fCardCmosRate[0]->Draw();
  temp->fCardTimeRate[0]->Draw();
  temp->fCardNhit[0]->Draw();
  temp->fTrigCount->Draw();
  temp->fTrigRate->Draw();
  TThread::UnLock();

  Int_t i=0;
  while(temp->IsFinished() == kFALSE){
    i++;
    if (i==upd) i=0;
    if ((i%upd) == 0) {
      switch(temp->GetCurrentTab()){
        case 0:
          temp->fNhit->Modified();
          temp->fNhitTimeRate->Modified();
          break;
        case 1: 
          for (Int_t j=0;j<20;j++)
            temp->fCCCHits[j]->Modified();
          break;
        case 2:
          for (Int_t j=0;j<20;j++){
            temp->fCCCRate[j]->Modified();
          }
          break;
        case 3:
          for (Int_t j=0;j<20;j++){
            temp->fCCCCmosRate[j]->Modified();
          }
          break;
        case 4:
          temp->fCrateHits[temp->GetCurrentCrate()]->Modified();
          temp->fCrateRate[temp->GetCurrentCrate()]->Modified();
          temp->fCrateCmosRate[temp->GetCurrentCrate()]->Modified();
          temp->fCrateTimeRate[temp->GetCurrentCrate()]->Modified();
          temp->fCrateNhit[temp->GetCurrentCrate()]->Modified();
          break;
        case 5:
          temp->fCardHits[temp->GetCurrentCard()]->Modified();
          temp->fCardRate[temp->GetCurrentCard()]->Modified();
          temp->fCardCmosRate[temp->GetCurrentCard()]->Modified();
          temp->fCardTimeRate[temp->GetCurrentCard()]->Modified();
          temp->fCardNhit[temp->GetCurrentCard()]->Modified();
        case 6:
          temp->fTrigCount->Modified();
          temp->fTrigRate->Modified();
          break;
      }
      usleep(1000000);
    }
  }
  printf("Draw thread exiting\n");
  return 0;
}

void *RootApp::DispatchThread(void* arg)
{
  RootApp* temp = (RootApp*) arg;
  avalanche::client client;
  client.addDispatcher("tcp://localhost:5024");
  while(temp->IsFinished() == kFALSE){
    RAT::DS::PackedRec* rec = (RAT::DS::PackedRec*) client.recv();
    if (rec){
      if (rec->RecordType == 1){
        RAT::DS::PackedEvent* event = (RAT::DS::PackedEvent*) rec->Rec;
        unsigned long clock10part1 = event->MTCInfo[0];
        unsigned long clock10part2 = event->MTCInfo[1] & 0x1FFFF;
        double seconds = ((clock10part2 << 32) + clock10part1)/10000000.;
        unsigned long trigtype = ((event->MTCInfo[3] & 0xFF000000)>>24) | ((event->MTCInfo[4] & 0x0003FFFF) << 8);

        for (Int_t i=0;i<26;i++){
          if ((0x1<<i) & trigtype){
            if (i<7){
              temp->fTrigCount->Fill(i);
              temp->fTrigRate->Fill(i,seconds);
            }
            if (i==10){
              temp->fTrigCount->Fill(7);
              temp->fTrigRate->Fill(7,seconds);
            }
            if (i==11){
              temp->fTrigCount->Fill(8);
              temp->fTrigRate->Fill(8,seconds);
            }
          }
        }

        printf("Got an event with nhit %u at time %f with mask %08x\n",event->NHits,seconds,(unsigned int) trigtype);
        temp->fNhit->Fill(event->NHits);
        temp->fNhitTimeRate->Fill(event->NHits,seconds);

        Int_t NhitPerCrate[20];
        Int_t NhitPerCard[320];
        for (Int_t i=0;i<20;i++)
          NhitPerCrate[i] = 0;
        for (Int_t i=0;i<320;i++)
          NhitPerCard[i] = 0;
        for (Int_t i=0;i<event->PMTBundles.size();i++){
          RAT::DS::PMTBundle bundle = event->PMTBundles[i];
          int crate = (bundle.Word[0] >> 21) & (((ULong64_t)1 << 5) - 1);
          int card = (bundle.Word[0] >> 26) & (((ULong64_t)1 << 4) - 1);
          int chan = (bundle.Word[0] >> 16) & (((ULong64_t)1 << 5) - 1);
          temp->fCCCHits[crate]->Fill(card,chan);
          temp->fCCCRate[crate]->Fill(card,chan,seconds);
          temp->fCCCCmosRate[crate]->Fill(card,chan,seconds); //FIXME

          temp->fCrateHits[crate]->Fill(card*32+chan);
          temp->fCrateRate[crate]->Fill(card*32+chan,seconds);
          temp->fCrateCmosRate[crate]->Fill(card*32+chan,seconds); //FIXME

          temp->fCardHits[crate*16+card]->Fill(chan);
          temp->fCardRate[crate*16+card]->Fill(chan,seconds);
          temp->fCardCmosRate[crate*16+card]->Fill(chan,seconds); //FIXME

          NhitPerCrate[crate]++;
          NhitPerCard[crate*16+card]++;
        }

        for (Int_t i=0;i<20;i++){
          temp->fCrateNhit[i]->Fill(NhitPerCrate[i]);
          temp->fCrateTimeRate[i]->Fill(NhitPerCrate[i],seconds);
        }
        for (Int_t i=0;i<320;i++){
          temp->fCardNhit[i]->Fill(NhitPerCard[i]);
          temp->fCardTimeRate[i]->Fill(NhitPerCard[i],seconds);
        }
      }
    }
    delete rec;
  }
  printf("Dispatch thread exiting\n");
  return 0;
}

int main(int argc, char *argv[])
{ 
  TApplication theApp("App", &argc, argv);
  // Popup the GUI... 
  RootApp *temp = new RootApp(gClient->GetRoot(),200,200); 
  theApp.Run();

  return 0;
}

