#ifndef ROO_HSSPLOTANDFIT_STUDY
#define ROO_HSSPLOTANDFIT_STUDY

#include "RooAbsStudy.h"

class RooAbsPdf;
class RooDataSet ;
class RooAbsData ;
class RooFitResult ;
class RooPlot ;
class RooRealVar ;
class RooWorkspace ;
class RooAbsGenContext ;
class THSRooFit;

#include "RooArgSet.h"
#include "RooLinkedList.h"
#include "RooAbsPdf.h"
#include "THSRooFit.h"
#include "THSsPlot.h"
 

class RooHSsPlotAndFitStudy : public RooAbsStudy {
public:

  RooHSsPlotAndFitStudy(const char* name=0, const char* title=0) ;
  RooHSsPlotAndFitStudy(const RooHSsPlotAndFitStudy& other) ;
  virtual ~RooHSsPlotAndFitStudy() ;
  virtual RooAbsStudy* clone(const char* newname="") const { return new RooHSsPlotAndFitStudy(*this) ; }

  virtual Bool_t attach(RooWorkspace& w) ;
  virtual Bool_t initialize() ;
  virtual Bool_t execute() ;
  virtual Bool_t finalize() ;
  virtual Bool_t terminate() ;

  virtual void GenHSRooFit(THSRooFit* rf){fGenRooFit=rf;}
  virtual void FitHSsPlot(THSsPlot* sp,TString species){sp->SetTitle(species);fsPlots->Add(sp);}
  virtual void FitHSRooFit(THSRooFit* rf){fHSRooFit=rf;}
  
  void SetPDFName(TString pdf){fPDFName=pdf;}
  void Print(Option_t *options= 0) const;
  RooWorkspace* GetWorkSpace(){return fWS;}
  THSRooFit* GetRooFit(){return fHSRooFit;};
  void SetOutDir(TString dir){fOutDir=dir;}
  TString GetOutDir(){return fOutDir;}
  void SetPlot(Bool_t plot=kTRUE){fIsPlot=plot;}
  void UseAll(Bool_t all=kTRUE){fUseAll=all;}
 protected:


  RooAbsPdf* _genPdf=0 ; //!
  RooArgSet _genObs ;  //!
  RooAbsPdf* _fitPdf=0 ; //!
  RooArgSet _fitObs ; //!

  RooRealVar* _nllVar=0 ; //!
  RooRealVar* _ngenVar=0 ; //!
  RooArgSet* _params=0 ; //!
  RooArgSet* _initParams=0; //!
  
  THSRooFit * fGenRooFit=0; 
  THSRooFit * fHSRooFit=0;
  TList* fsPlots=0; //can do more than 1 splot
  RooWorkspace* fWS=0; //!
  TString fOutDir;
  
  Bool_t fIsCloned=kFALSE;
  Bool_t fIsPlot=kFALSE;
  Bool_t fUseAll=kFALSE;
  TString fPDFName;
  Long64_t fGeni=0;
  
  ClassDef(RooHSsPlotAndFitStudy,1) // Generate-and-Fit study module
} ;


#endif

