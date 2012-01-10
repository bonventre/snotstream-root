#include <TGClient.h> 
#include <TCanvas.h>  
#include <TH1.h> 
#include <TH2.h> 
#include <TRandom.h> 
#include <TGButton.h> 
#include <TGMenu.h>
#include <TSystem.h>
#include <TThread.h>
#include <TApplication.h>
#include <TGTableLayout.h>
#include <TGFrame.h>
#include <TGTab.h>
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

  int nfills = 250000;
  int upd = 5000;

  TThread::Lock();
  for (Int_t i=0;i<20;i++){
    fCanvas[i]->cd(); 
    f1[i]->Draw("LEGO"); 
    fCanvasrate[i]->cd();
    f1rate[i]->Draw("LEGO");
  }
  fCanvasnhit->cd();
  f1nhit->Draw();
  TThread::UnLock();
//  srand ( time(NULL) );

    /* generate secret number: */
//  Float_t px, py, pz;
  Int_t i=0;
  while(1){
    i++;
    if (i==upd) i=0;
//    pz = px*px + py*py;
    //px = i%10000;
//    px = rand() % 10000;
//    f1->Fill(px);
    if ((i%upd) == 0) {
      //CURLcode res;
      //res = curl_easy_perform(curl);
      if (currentTab == 1)
        for (Int_t j=0;j<20;j++)
          fCanvas[j]->Modified();
      if (currentTab == 0)
        fCanvasnhit->Modified();
      if (currentTab == 2){
        for (Int_t j=0;j<20;j++){
          if (endtime[j] > 0){
            double elapsed = endtime[j]-starttime[j];
            for (Int_t k=0;k<16;k++){
              for (Int_t l=0;l<32;l++){
                  f1rate[j]->SetBinContent(k+1,l+1,(hits[j*16*32+k*32+l]/elapsed));
              }
            }
          }
          fCanvasrate[j]->Modified();
        }
      }
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

void mainFrame::DoTab(Int_t id)
{
  currentTab = id;
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
        f1nhit->Fill(event->NHits);
        unsigned long clock10part1 = event->MTCInfo[0];
        unsigned long clock10part2 = event->MTCInfo[1] & 0x1FFFF;
        double seconds = ((clock10part2 << 32) + clock10part1)/10000000.;
        for (Int_t i=0;i<event->PMTBundles.size();i++){
          RAT::DS::PMTBundle bundle = event->PMTBundles[i];
          int crate = (bundle.Word[0] >> 21) & (((ULong64_t)1 << 5) - 1);
          int card = (bundle.Word[0] >> 26) & (((ULong64_t)1 << 4) - 1);
          int chan = (bundle.Word[0] >> 16) & (((ULong64_t)1 << 5) - 1);
          f1[crate]->Fill(card,chan);
          hits[crate*16*32+card*32+chan]++;
          if (starttime[crate] == 0)
            starttime[crate] = seconds;
          else
            endtime[crate] = seconds;
        }
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
  currentTab = 0;

  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "http://nutau.hep.upenn.edu:8051/data?nhit=-1");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &curl_write_func);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1);
  }

  for (Int_t i=0;i<20;i++){
    char tempname[10];
    sprintf(tempname,"f%02d",i);
    f1[i] = new TH2F(tempname,"This is the Nhit distribution",16,0,16,32,0,32);
    sprintf(tempname,"f%02d rate",i);
    f1rate[i] = new TH2F(tempname,"This is the Nhit distribution",16,0,16,32,0,32);
    starttime[i] = 0;
    endtime[i] = 0;
  }
  f1nhit = new TH1F("nhit","Nhit",20,0,400);
  for (Int_t i=0;i<10000;i++){
    hits[i] = 0;
  }
  


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

  TGTab *tabframe = new TGTab(fMain,100,100);
  tabframe->Connect("Selected(Int_t)", "mainFrame",this,"DoTab(Int_t)");

  TGCompositeFrame *cframe2 = tabframe->AddTab("NHit");
  cframe2->SetLayoutManager(new TGTableLayout(cframe2,2,1));
  fEcanvasnhit = new TRootEmbeddedCanvas("nhit",cframe2,100,100); 
  cframe2->AddFrame(fEcanvasnhit,new TGTableLayoutHints(0,1,0,1,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));


  TGCompositeFrame *cframe = tabframe->AddTab("Hits per Channel");
  cframe->SetLayoutManager(new TGTableLayout(cframe,5,4));
  for (Int_t i=0;i<5;i++){
    for (Int_t j=0;j<4;j++){
      char tempname[10];
      sprintf(tempname,"%02d",i*4+j);
      fEcanvas[i*4+j] = new TRootEmbeddedCanvas(tempname,cframe,100,100); 
      cframe->AddFrame(fEcanvas[i*4+j],new TGTableLayoutHints(j,j+1,i,i+1,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
    }
  }

  TGCompositeFrame *cframe3 = tabframe->AddTab("Hit Rate per Channel");
  cframe3->SetLayoutManager(new TGTableLayout(cframe3,5,4));
  for (Int_t i=0;i<5;i++){
    for (Int_t j=0;j<4;j++){
      char tempname[10];
      sprintf(tempname,"%02d rate",i*4+j);
      fEcanvasrate[i*4+j] = new TRootEmbeddedCanvas(tempname,cframe3,100,100); 
      cframe3->AddFrame(fEcanvasrate[i*4+j],new TGTableLayoutHints(j,j+1,i,i+1,kLHintsFillX | kLHintsFillY | kLHintsShrinkX | kLHintsShrinkY | kLHintsExpandX | kLHintsExpandY));
    }
  }



  
  fMain->AddFrame(tabframe, new TGLayoutHints(kLHintsExpandX| kLHintsExpandY, 
        10,10,10,1));  

  // Set a name to the main frame 
  fMain->SetWindowName("Simple Example"); 
  // Map all subwindows of main frame 
  fMain->MapSubwindows(); 
  // Initialize the layout algorithm 
  fMain->Resize(fMain->GetDefaultSize()); 
  // Map main frame 
  fMain->MapWindow(); 

  for (Int_t i=0;i<20;i++){
    fCanvas[i] = fEcanvas[i]->GetCanvas();  
    fCanvasrate[i] = fEcanvasrate[i]->GetCanvas();  
  }
  fCanvasnhit = fEcanvasnhit->GetCanvas();

  while (!finished) {
    for (Int_t i=0;i<20;i++){
      if (fCanvas[i]->IsModified())
        fCanvas[i]->Update();
      if (fCanvasrate[i]->IsModified())
        fCanvasrate[i]->Update();
    }
    if (fCanvasnhit->IsModified())
      fCanvasnhit->Update();

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


