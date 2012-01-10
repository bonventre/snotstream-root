#include <TGClient.h> 
#include <TCanvas.h>  
#include <TH1.h> 
#include <TRandom.h> 
#include <TGButton.h> 
#include <TGMenu.h>
#include <TSystem.h>
#include <TThread.h>
#include <TApplication.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <curl/curl.h>
#include <jsoncpp/json.h>
#include <RAT/DS/PackedEvent.hh>
#include <avalanche.hpp>

#include "mainFrame.h"
#include "curlstuff.h"

void mainFrame::DoDraw() { 
  if (!thread){
    thread = new TThread("memberfunction",(void(*) (void *))&mainFrame::thread_draw,(void *)this);
    thread->Run();
  }
}

void *mainFrame::thread_draw(void* arg){
  // Draws function graphics in randomly choosen interval 
  TCanvas *fCanvas = fEcanvas->GetCanvas();  

  int nfills = 250000;
  int upd = 5000;

  TThread::Lock();
  fCanvas->cd(); 
  f1->Draw(); 
  TThread::UnLock();
//  srand ( time(NULL) );

    /* generate secret number: */
//  Float_t px, py, pz;
  for (Int_t i = 0; i < nfills; i++) {
//    pz = px*px + py*py;
    //px = i%10000;
//    px = rand() % 10000;
//    f1->Fill(px);
    if ((i%upd) == 0) {
      CURLcode res;
      res = curl_easy_perform(curl);
      fCanvas->Modified();
      usleep(1000000);
    }
  }
  printf("exiting\n");
}

void mainFrame::HandleMenu(Int_t id)
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
  }
}

void mainFrame::CloseWindow()
{
  if (AvalancheThread){
    finished = kTRUE;
    AvalancheThread->Join();
  }
  gApplication->Terminate();
}

void *mainFrame::thread_avalanche(void* arg)
{
  avalanche::client client("tcp://localhost:5024");
  while(finished == kFALSE){
    RAT::DS::PackedRec* rec = (RAT::DS::PackedRec*) client.recvObject(RAT::DS::PackedRec::Class());
    if (rec){
      if (rec->RecordType == 1){
        RAT::DS::PackedEvent* event = (RAT::DS::PackedEvent*) rec->Rec;
        printf("Got an event with nhit %u\n",event->NHits);
        f1->Fill(event->NHits);
      }
    }
    delete rec;
  }
  return 0;
}

void mainFrame::curl_write(char *ptr)
{
  printf("got back '%s'\n",ptr);
  Json::Value root;   // will contains the root value after parsing.
  Json::Reader reader;
  reader.parse(ptr,root);
  std::cout << root.get("nhit","TEST").toStyledString();
}

mainFrame::mainFrame(const TGWindow *p,UInt_t w,UInt_t h) { 
  finished = kFALSE;
  thread = 0;
  lastkey = -1;

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://nutau.hep.upenn.edu:8051/data?nhit=-1");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curl_write_func);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
  }

  f1 = new TH1F("f1","This is the Nhit distribution",50,0,1000);
  AvalancheThread = new TThread("avalanche",(void(*) (void *))&mainFrame::thread_avalanche,(void *)this);
  AvalancheThread->Run();


  // Create a main frame 
  fMain = new TGMainFrame(p,w,h); 

  // create a menu widget
  TGMenuBar *fMenuBar = new TGMenuBar(fMain,100,20,kHorizontalFrame);

  TGPopupMenu *fMenuFile = new TGPopupMenu(gClient->GetRoot());
  fMenuFile->AddEntry("&Open",M_FILE_OPEN);
  fMenuFile->AddEntry("&Draw Plot",M_FILE_DRAW_PLOT);
  fMenuFile->AddSeparator();
  fMenuFile->AddEntry("&Exit",M_FILE_EXIT);
  fMenuFile->Connect("Activated(Int_t)","mainFrame",this,"HandleMenu(Int_t)");
  fMenuBar->AddPopup("&File",fMenuFile,new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0) );

  TGPopupMenu *fMenuEdit = new TGPopupMenu(gClient->GetRoot());
  fMenuEdit->AddEntry("&Copy",2);
  fMenuBar->AddPopup("&Edit",fMenuEdit,new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0) );

  TGPopupMenu *fMenuHelp = new TGPopupMenu(gClient->GetRoot());
  fMenuHelp->AddEntry("&About",1);
  fMenuBar->AddPopup("&Help",fMenuHelp,new TGLayoutHints(kLHintsTop | kLHintsRight) );
  fMain->AddFrame(fMenuBar,new TGLayoutHints(kLHintsTop | kLHintsExpandX));

  // Create canvas widget 
  fEcanvas = new TRootEmbeddedCanvas("Ecanvas",fMain,200,200); 
  fMain->AddFrame(fEcanvas, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY, 
        10,10,10,1));  

  // Set a name to the main frame 
  fMain->SetWindowName("Simple Example"); 
  // Map all subwindows of main frame 
  fMain->MapSubwindows(); 
  // Initialize the layout algorithm 
  fMain->Resize(fMain->GetDefaultSize()); 
  // Map main frame 
  fMain->MapWindow(); 

  TCanvas *fCanvas = fEcanvas->GetCanvas();  

  while (!finished) {
    if (fCanvas->IsModified()) {
      printf("Update canvas\n");
      fCanvas->Update();
    }
    gSystem->ProcessEvents();
    usleep(100);
  }
}

mainFrame::~mainFrame() { 
  // Clean up used widgets: frames, buttons, layouthints 
  fMain->Cleanup(); 
  curl_easy_cleanup(curl);
  delete fMain; 
}


