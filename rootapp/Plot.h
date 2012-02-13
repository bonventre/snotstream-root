#ifndef PLOT_H
#define PLOT_H

#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <vector>

#include <RQ_OBJECT.h> 

class Plot {
  RQ_OBJECT("Plot")

  protected:
    Bool_t              fPaused;

    void                *fApp;

    TRootEmbeddedCanvas *fECanvas;
    TCanvas             *fCanvas;

  public:
    Plot(void* app);
    ~Plot();

    void CreateECanvas(const char* name = 0, const TGWindow* p = 0, UInt_t w = 10, UInt_t h = 10);
    void SetECanvas(TRootEmbeddedCanvas *ECanvas);
    TRootEmbeddedCanvas *GetECanvas();
    void Update();
    void Modified();
    void Pause();
    void UnPause();
};

class HistPlot : public Plot {
  RQ_OBJECT("HistPlot") 

  private:
    TH1F                *fHist; 
    Int_t               fXbins;

  public:
    HistPlot(void* app, const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup);
    ~HistPlot();

    void Draw();
    void Fill(Double_t x);
    void SetBinLabels(char ticks[][30]);
    void Clear();
};

class TimeRatePlot : public Plot {
  RQ_OBJECT("TimeRatePlot")

  private:
    TH1F                *fHist;

    Double_t            fFirstBinTime;
    Double_t            fFirstBinCount;
    Int_t               fXbins;

  public:
    TimeRatePlot(void* app, const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup);
    ~TimeRatePlot();

    void Draw();
    void Fill(Double_t counts, Double_t t);
    void Clear();
};

class Hist2dPlot : public Plot {
  RQ_OBJECT("Hist2dPlot") 

  private:
    TH2F                *fHist; 

  public:
    Hist2dPlot(void* app, const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup);
    ~Hist2dPlot();

    void Draw(const char* option);
    void Fill(Double_t x, Double_t y);
    void Clear();
};

class RatePlot : public Plot {
  RQ_OBJECT("RatePlot") 

  private:
    TH1F                *fHist; 

    Int_t               fXbins;
    Double_t            fStartTime;
    Double_t            fCurrentTime;
//    std::vector<Double_t> fCounts;
    Double_t            fCounts[512];
    Double_t            fOldRates[512];
    Double_t            fOldWeight;

  public:
    RatePlot(void* app, const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup);
    ~RatePlot();

    void Fill(Double_t x, Double_t t);
    void Draw(const char* option = "");
    void Modified();
    void SetBinLabels(char ticks[][30]);
    void Clear();
    void Pause();
};


class Rate2dPlot : public Plot {
  RQ_OBJECT("Rate2dPlot") 

  private:
    TH2F                *fHist; 

    Int_t               fXbins;
    Int_t               fYbins;
    Double_t            fStartTime;
    Double_t            fCurrentTime;
//    std::vector<Double_t> fCounts;
    Double_t            fCounts[512];
    Double_t            fOldRates[512];
    Double_t            fOldWeight;

  public:
    Rate2dPlot(void* app, const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup);
    ~Rate2dPlot();

    void Fill(Double_t x, Double_t y, Double_t t);
    void Draw(const char* option);
    void Modified();
    void Clear();
    void Pause();
};

#endif
