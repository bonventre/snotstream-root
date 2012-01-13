#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>

#include "Plot.h"

Plot::Plot(void* app)
{
  fPaused = kFALSE;
  fApp = app;
}

Plot::~Plot()
{
  delete fECanvas;
}

void Plot::CreateECanvas(const char* name, const TGWindow* p, UInt_t w, UInt_t h)
{
  fECanvas = new TRootEmbeddedCanvas(name,p,w,h);
  fCanvas = fECanvas->GetCanvas();
  fCanvas->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)","RootApp",fApp,"EventInfo(Int_t,Int_t,Int_t,TObject*)");
}

void Plot::SetECanvas(TRootEmbeddedCanvas *ECanvas)
{
  fECanvas = ECanvas;
  fCanvas = fECanvas->GetCanvas();
  fCanvas->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)","RootApp",fApp,"EventInfo(Int_t,Int_t,Int_t,TObject*)");
}

TRootEmbeddedCanvas *Plot::GetECanvas()
{
  return fECanvas;
}

void Plot::Update()
{
  if (fCanvas->IsModified())
    fCanvas->Update();
}

void Plot::Clear(){}

void Plot::Modified()
{
  fCanvas->Modified();
}

void Plot::Pause()
{
  fPaused = kTRUE;
}

void Plot::UnPause()
{
  fPaused = kFALSE;
}

HistPlot::HistPlot(void* app, const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup)
  : Plot(app)
{
  fHist = new TH1F(name,title,nbinsx,xlow,xup);
  fXbins = nbinsx;
}

HistPlot::~HistPlot()
{
  delete fHist;
}

void HistPlot::Draw()
{
  fCanvas->cd();
  fHist->Draw();
}

void HistPlot::Fill(Double_t x)
{
  if (fPaused == kFALSE)
    fHist->Fill(x);
}

void HistPlot::SetBinLabels(char ticks[][30])
{
  for (Int_t i=0;i<fXbins;i++){
    fHist->GetXaxis()->SetBinLabel(i+1,ticks[i]);
  }
}

TimeRatePlot::TimeRatePlot(void* app, const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup)
  : Plot(app)
{
  fHist = new TH1F(name,title,nbinsx,xlow,xup);
  fFirstBinTime = 0;
  fFirstBinCount = 0;
  fXbins = nbinsx;
}


TimeRatePlot::~TimeRatePlot()
{
  delete fHist;
}

void TimeRatePlot::Draw()
{
  fCanvas->cd();
  fHist->Draw();
}

void TimeRatePlot::Fill(Double_t counts, Double_t t)
{
  if (fPaused == kFALSE){
    if (fFirstBinTime == 0)
      fFirstBinTime = t;
    if ((t-fFirstBinTime) < 1){
      fFirstBinCount += counts;
    }else{
      int howmany = (int) (t-fFirstBinTime);
      for (Int_t j=1;j<fXbins;j++){
        if (j+howmany > fXbins)
          fHist->SetBinContent(j,0);
        else{
          double bc = fHist->GetBinContent(j+howmany+1);
          fHist->SetBinContent(j+1,bc);
        }
      }
      fHist->SetBinContent(fXbins,fFirstBinCount);
      fFirstBinTime += howmany;
      fFirstBinCount = counts;
    }
  }
}

Hist2dPlot::Hist2dPlot(void* app, const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup)
  : Plot(app)
{
  fHist = new TH2F(name,title,nbinsx,xlow,xup,nbinsy,ylow,yup);
  printf("made at %p\n",fHist);
}

Hist2dPlot::~Hist2dPlot()
{
  delete fHist;
}

void Hist2dPlot::Draw(const char* option)
{
  fCanvas->cd();
  fHist->Draw(option);
}

void Hist2dPlot::Fill(Double_t x,Double_t y)
{
  if (fPaused == kFALSE)
    fHist->Fill(x,y);
}

Rate2dPlot::Rate2dPlot(void* app, const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup)
  : Plot(app)
{
  fHist = new TH2F(name,title,nbinsx,xlow,xup,nbinsy,ylow,yup);
  fStartTime = 0;
  fCurrentTime = 0;
  fXbins = nbinsx;
  fYbins = nbinsy;
  for (Int_t i=0;i<512;i++)
    fCounts[i] = 0;
}

Rate2dPlot::~Rate2dPlot()
{
  delete fHist;
}

void Rate2dPlot::Draw(const char* option)
{
  fCanvas->cd();
  fHist->Draw(option);
}

void Rate2dPlot::Modified()
{
  if (fCurrentTime > 0){
    double elapsed = fCurrentTime-fStartTime;
    for (Int_t i=0;i<fXbins;i++){
      for (Int_t j=0;j<fYbins;j++){
        fHist->SetBinContent(i+1,j+1,(fCounts[i*fYbins+j]/elapsed));
      }
    }
  }
  fCanvas->Modified();
}

void Rate2dPlot::Fill(Double_t x, Double_t y, Double_t t)
{
  if (fPaused == kFALSE){
    fCounts[(int)x*fYbins+(int)y]++;
    if (fStartTime == 0)
      fStartTime = t;
    else
      fCurrentTime = t;
  }
}

BarPlot::BarPlot(void* app, const char* name, const char* title, Int_t nbinsx, const char ticks[][30])
  : Plot(app)
{
  fHist = new TH1F(name,title,nbinsx,0,nbinsx);
  fHist->SetMinimum(0);
  for (Int_t i=0;i<nbinsx;i++){
    fHist->GetXaxis()->SetBinLabel(i+1,ticks[i]);
  }
}

BarPlot::~BarPlot()
{
  delete fHist;
}

void BarPlot::Draw()
{
  fCanvas->cd();
  fHist->Draw();
}

void BarPlot::Fill(Double_t x)
{
  if (fPaused == kFALSE)
    fHist->Fill(x);
}


