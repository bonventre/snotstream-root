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
#include "jsoncpp/json.h"
#include "RAT/DS/PackedEvent.hh"
#include "avalanche.hpp"

#include "RootApp.h"

enum menu_identifiers {
  M_FILE_OPEN,
  M_FILE_DRAW_PLOT,
  M_FILE_EXIT,
  M_EDIT_CLEAR_ALL,
  M_EDIT_PAUSE,
  M_EDIT_START
};

RootApp::RootApp(const TGWindow *p,UInt_t w,UInt_t h) : TGMainFrame(p,w,h) { 
  fFinished = kFALSE;
  fCurrentTab = 0;
  fDrawThread = 0;

  for (Int_t i=0;i<20;i++){
    char tempname[10];
    sprintf(tempname,"Crate %02d hits",i);
    fCCCHits[i] = new Hist2dPlot(tempname,"Hits per channel",16,0,16,32,0,32);
    sprintf(tempname,"Crate %02d rate",i);
    fCCCRate[i] = new Rate2dPlot(tempname,"Hit Rate (hits/s) per channel",16,0,16,32,0,32);
  }
  fNhit = new HistPlot("Nhit","Nhit",20,0,400);
  fNhitRate = new TimeRatePlot("Nhit rate","Scrolling average total hit rate (hits/s)",20,-20,0);

  fDispatchThread = new TThread("dispatch",(void* (*) (void *)) &RootApp::DispatchThread,(void*) this);
  fDispatchThread->Run();

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

  //fToolTip = new TGToolTip(fClient->GetDefaultRoot(),fEcanvasnhit,"",250);
  //fCanvasnhit->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)","mainFrame",this,"EventInfo(Int_t,Int_t,Int_t,TObject*)");

  while (!fFinished) {
    for (Int_t i=0;i<20;i++){
      fCCCHits[i]->Update();
      fCCCRate[i]->Update();
    }
    fNhit->Update();
    fNhitRate->Update();

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
  delete fNhit;
  delete fNhitRate;
  for (Int_t i=0;i<20;i++){
    delete fCCCHits[i];
    delete fCCCRate[i];
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
  fMenuFile->AddEntry("&Exit",M_FILE_EXIT);
  fMenuFile->Connect("Activated(Int_t)","RootApp",this,"HandleMenu(Int_t)");
  fMenuBar->AddPopup("&File",fMenuFile,new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0) );

  fMenuEdit = new TGPopupMenu(gClient->GetRoot());
  fMenuEdit->AddEntry("&Clear All Plots",M_EDIT_CLEAR_ALL);
  fMenuEdit->AddSeparator();
  fMenuEdit->AddEntry("&Pause",M_EDIT_PAUSE);
  fMenuEdit->AddEntry("&Start",M_EDIT_START);
  fMenuEdit->DisableEntry(M_EDIT_PAUSE);
  fMenuEdit->DisableEntry(M_EDIT_START);
  fMenuEdit->Connect("Activated(Int_t)","RootApp",this,"HandleMenu(Int_t)");
  fMenuBar->AddPopup("&Edit",fMenuEdit,new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0) );

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
  fNhit->SetECanvas("nhit",fTab1,100,100); 
  fTab1->AddFrame(fNhit->GetECanvas(),new TGTableLayoutHints(0,1,0,1,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
  fNhitRate->SetECanvas("nhit rate",fTab1,100,100); 
  fTab1->AddFrame(fNhitRate->GetECanvas(),new TGTableLayoutHints(0,1,1,2,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));

  fTab2 = fDisplayFrame->AddTab("Hits per Channel");
  fTab2->SetLayoutManager(new TGTableLayout(fTab2,5,4));
  for (Int_t i=0;i<5;i++){
    for (Int_t j=0;j<4;j++){
      char tempname[10];
      sprintf(tempname,"%02d",i*4+j);
      fCCCHits[i*4+j]->SetECanvas(tempname,fTab2,100,100); 
      fTab2->AddFrame(fCCCHits[i*4+j]->GetECanvas(),new TGTableLayoutHints(j,j+1,i,i+1,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
    }
  }

  fTab3 = fDisplayFrame->AddTab("Hit Rate per Channel");
  fTab3->SetLayoutManager(new TGTableLayout(fTab3,5,4));
  for (Int_t i=0;i<5;i++){
    for (Int_t j=0;j<4;j++){
      char tempname[10];
      sprintf(tempname,"%02d rate",i*4+j);
      fCCCRate[i*4+j]->SetECanvas(tempname,fTab3,100,100); 
      fTab3->AddFrame(fCCCRate[i*4+j]->GetECanvas(),new TGTableLayoutHints(j,j+1,i,i+1,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
    }
  }

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
      fNhit->Clear();
      fNhitRate->Clear();
      for (Int_t i=0;i<20;i++){
        fCCCHits[i]->Clear();
        fCCCRate[i]->Clear();
      }
      break;
    case M_EDIT_PAUSE:
      fNhit->Pause();
      fNhitRate->Pause();
      for (Int_t i=0;i<20;i++){
        fCCCHits[i]->Pause();
        fCCCRate[i]->Pause();
      }
      fMenuEdit->DisableEntry(M_EDIT_PAUSE);
      fMenuEdit->EnableEntry(M_EDIT_START);
      break;
    case M_EDIT_START:
      fNhit->UnPause();
      fNhitRate->UnPause();
      for (Int_t i=0;i<20;i++){
        fCCCHits[i]->UnPause();
        fCCCRate[i]->UnPause();
      }
      fMenuEdit->DisableEntry(M_EDIT_START);
      fMenuEdit->EnableEntry(M_EDIT_PAUSE);
      break;
  }
}

void RootApp::EventInfo(Int_t event, Int_t px, Int_t py, TObject *selected)
{
  /*
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
  */
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
    fCCCHits[i]->Draw("LEGO");
    fCCCRate[i]->Draw("LEGO");
  }
  fNhit->Draw();
  fNhitRate->Draw();
  TThread::UnLock();

  Int_t i=0;
  while(IsFinished() == kFALSE){
    //if (paused == kFALSE){
      i++;
      if (i==upd) i=0;
      if ((i%upd) == 0) {
        if (GetCurrentTab() == 1)
          for (Int_t j=0;j<20;j++)
            fCCCHits[j]->Modified();
        if (GetCurrentTab() == 0){
          fNhit->Modified();
          fNhitRate->Modified();
        }
        if (GetCurrentTab() == 2){
          for (Int_t j=0;j<20;j++){
            fCCCRate[j]->Modified();
          }
        }
        usleep(1000000);
      }
    //}
  }
  printf("Draw thread exiting\n");
  return 0;
}

void *RootApp::DispatchThread(void* arg)
{
  RootApp* temp = (RootApp*) arg;
  avalanche::client client("tcp://localhost:5024");
  while(IsFinished() == kFALSE){
    RAT::DS::PackedRec* rec = (RAT::DS::PackedRec*) client.recvObject(RAT::DS::PackedRec::Class(),ZMQ_NOBLOCK);
    if (rec){
      if (rec->RecordType == 1){
        RAT::DS::PackedEvent* event = (RAT::DS::PackedEvent*) rec->Rec;
        unsigned long clock10part1 = event->MTCInfo[0];
        unsigned long clock10part2 = event->MTCInfo[1] & 0x1FFFF;
        double seconds = ((clock10part2 << 32) + clock10part1)/10000000.;

        printf("Got an event with nhit %u at time %f\n",event->NHits,seconds);
        fNhit->Fill(event->NHits);
        fNhitRate->Fill(event->NHits,seconds);

        for (Int_t i=0;i<event->PMTBundles.size();i++){
          RAT::DS::PMTBundle bundle = event->PMTBundles[i];
          int crate = (bundle.Word[0] >> 21) & (((ULong64_t)1 << 5) - 1);
          int card = (bundle.Word[0] >> 26) & (((ULong64_t)1 << 4) - 1);
          int chan = (bundle.Word[0] >> 16) & (((ULong64_t)1 << 5) - 1);
          fCCCHits[crate]->Fill(card,chan);
          fCCCRate[crate]->Fill(card,chan,seconds);
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

