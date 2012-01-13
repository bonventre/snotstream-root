#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>

#include "Plot.h"

Plot::Plot()
{
  fPaused = kFALSE;
}

Plot::~Plot()
{
  delete fECanvas;
}

void Plot::CreateECanvas(const char* name, const TGWindow* p, UInt_t w, UInt_t h)
{
  fECanvas = new TRootEmbeddedCanvas(name,p,w,h);
  fCanvas = fECanvas->GetCanvas();
}

void Plot::SetECanvas(TRootEmbeddedCanvas *ECanvas)
{
  fECanvas = ECanvas;
  fCanvas = fECanvas->GetCanvas();
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

HistPlot::HistPlot(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup)
{
  fHist = new TH1F(name,title,nbinsx,xlow,xup);
}

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

TimeRatePlot::TimeRatePlot(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup)
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

Hist2dPlot::Hist2dPlot(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup)
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

Rate2dPlot::Rate2dPlot(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup)
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
