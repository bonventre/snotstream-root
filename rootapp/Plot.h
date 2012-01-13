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

    TRootEmbeddedCanvas *fECanvas;
    TCanvas             *fCanvas;

  public:
    Plot();
    ~Plot();

    void SetECanvas(const char* name = 0, const TGWindow* p = 0, UInt_t w = 10, UInt_t h = 10);
    TRootEmbeddedCanvas *GetECanvas();
    void Update();
    void Clear();
    void Modified();
};

class HistPlot : public Plot {
  RQ_OBJECT("HistPlot") 

  private:
    TH1F                *fHist; 

  public:
    HistPlot(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup);
    ~HistPlot();

    void Draw();
    void Fill(Double_t x);
};

class TimeRatePlot : public Plot {
  RQ_OBJECT("TimeRatePlot")

  private:
    TH1F                *fHist;

    Double_t            fFirstBinTime;
    Double_t            fFirstBinCount;
    Int_t               fXbins;

  public:
    TimeRatePlot(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup);
    ~TimeRatePlot();

    void Draw();
    void Fill(Double_t counts, Double_t t);
};

class Hist2dPlot : public Plot {
  RQ_OBJECT("Hist2dPlot") 

  private:
    TH2F                *fHist; 

  public:
    Hist2dPlot(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup);
    ~Hist2dPlot();

    void Draw(const char* option);
    void Fill(Double_t x, Double_t y);
};

class Rate2dPlot : public Plot {
  RQ_OBJECT("Rate2dPlot") 

  private:
    TH2F                *fHist; 

    Int_t               fXbins;
    Int_t               fYbins;
    Double_t            fStartTime;
    Double_t            fCurrentTime;
    std::vector<Double_t> fCounts;

  public:
    Rate2dPlot(const char* name, const char* title, Int_t nbinsx, Double_t xlow, Double_t xup, Int_t nbinsy, Double_t ylow, Double_t yup);
    ~Rate2dPlot();

    void Fill(Double_t x, Double_t y, Double_t t);
    void Draw(const char* option);
    void Modified();
};
