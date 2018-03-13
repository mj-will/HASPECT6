

#include "RooFit.h"
#include "Riostream.h"

#include "RooHSsPlotAndFitStudy.h"
#include "RooHSEventsPDF.h"
#include "RooRandom.h"
#include "RooWorkspace.h"
#include "RooMsgService.h" 
#include "RooDataSet.h"
#include "RooAbsPdf.h"
#include "RooRealVar.h"
#include "RooGlobalFunc.h"
#include "RooFitResult.h"
#include "RooPullVar.h"
#include "RooStats/RooStatsUtils.h"
#include "THSWeights.h"

using namespace std ;

ClassImp(RooHSsPlotAndFitStudy)


//_____________________________________________________________________________
RooHSsPlotAndFitStudy::RooHSsPlotAndFitStudy(const char* name, const char* title) : 
  RooAbsStudy(name,title)
{  
   // Constructor
  fIsCloned=kFALSE;
  fsPlots=new TList();
  
}



//_____________________________________________________________________________
RooHSsPlotAndFitStudy::RooHSsPlotAndFitStudy(const RooHSsPlotAndFitStudy& other) : 
  RooAbsStudy(other)
{
  fIsCloned=kTRUE;
  fHSRooFit=new THSRooFit((THSRooFit*)other.fHSRooFit);
  fGenRooFit=new THSRooFit((THSRooFit*)other.fGenRooFit);
  fsPlots=new TList();
  fsPlots->SetOwner(kTRUE);
  for(Int_t isp=0;isp<other.fsPlots->GetEntries();isp++){
    THSsPlot* sp=new THSsPlot((THSsPlot*)fsPlots->At(isp));
    fsPlots->Add(sp);
  }
  fWS=fHSRooFit->GetWorkSpace();
  fOutDir=other.fOutDir;
  fIsPlot=other.fIsPlot;
  fUseAll=other.fUseAll;
}



//_____________________________________________________________________________
RooHSsPlotAndFitStudy::~RooHSsPlotAndFitStudy()
{
  if(fIsCloned){
    if(fHSRooFit) {
      delete fHSRooFit;
      delete fGenRooFit;
      delete fsPlots;
      fHSRooFit=0;
      fGenRooFit=0;
      fsPlots=0;
    }
  }

}
Bool_t RooHSsPlotAndFitStudy::attach(RooWorkspace& w) {
  cout<<"RooHSsPlotAndFitStudy::attach "<<endl;
  fWS=&w;
  fHSRooFit->LoadWorkSpace(&w);
  fHSRooFit->SetOutDir(fOutDir);
  fHSRooFit->SetPlot(fIsPlot);
  fGenRooFit->LoadWorkSpace(&w);
  fGenRooFit->SetOutDir(fOutDir);
  fGenRooFit->SetPlot(fIsPlot);
  // fGenRooFit->TotalPDF();
  for(Int_t isp=0;isp<fsPlots->GetEntries();isp++){
    THSsPlot* sp=((THSsPlot*)fsPlots->At(isp));
    sp->LoadWorkSpace(&w);
    sp->SetOutDir(fOutDir);
    sp->SetPlot(fIsPlot);
    sp->TotalPDF();
   
  }
  return kTRUE;
}


//_____________________________________________________________________________
Bool_t RooHSsPlotAndFitStudy::initialize() 
{ 
  // One-time initialization of study 

  cout<<"RooHSsPlotAndFitStudy::intialise "<<endl;
  _nllVar = new RooRealVar("NLL","-log(Likelihood)",0) ;
  _ngenVar = new RooRealVar("ngen","number of generated events",0) ;
  
  _params = new RooArgSet(fHSRooFit->GetParameters()) ;
  RooArgSet modelParams(*_params) ;
  modelParams.add(fHSRooFit->GetYields());
  fWS->saveSnapshot("initial",modelParams,kTRUE);
  _initParams = (RooArgSet*) _params->snapshot() ;
  _params->add(*_nllVar) ;
  _params->add(*_ngenVar) ;

  registerSummaryOutput(*_params,modelParams) ;
  
  fHSRooFit->AddFitOption(RooFit::SumW2Error(kTRUE));
   return kFALSE ;
} 

//_____________________________________________________________________________
Bool_t RooHSsPlotAndFitStudy::execute() 
{
  cout<<"RooHSsPlotAndFitStudy::execute() "<<fGenRooFit<<" "<<fHSRooFit<<" "<<fsPlots->GetEntries()<<endl;
  cout<<fGenRooFit->GetModel()<<endl;
  fGenRooFit->GetModel()->Print();
  //generate pseudo data set
  fWS->loadSnapshot("initial");//load initial parameters
  //fGenRooFit->TotalPDF();
  //Number of events to generate
  Long64_t nexp=RooRandom::randomGenerator()->Poisson(fGenRooFit->GetModel()->expectedEvents(fGenRooFit->GetVariables()));
  //
  fGenRooFit->GetModel()->Print();
  fGenRooFit->GetVariables().Print();
  //Generate a full signal+ background data set
  RooDataSet* DS=fGenRooFit->GetModel()->generate(fGenRooFit->GetVariables(),nexp);
  Info("RooHSsPlotAndFitStudy::execute() ","generated pseudo data :");
  fGenRooFit->GetVariables().Print("v");
  DS->Print();

  //Loop over splots
  THSWeights* lastWeights=nullptr;
  THSsPlot* sp=nullptr;
  for(Int_t isp=0;isp<fsPlots->GetEntries();isp++){
    sp= dynamic_cast<THSsPlot*>(fsPlots->At(isp));
     //load new data
    sp->LoadDataSet(DS);
    //do splot fit
    sp->SetSaveWeights(kFALSE);
    if(lastWeights) {
      sp->LoadWeights(lastWeights);
      sp->SetWeightName(sp->GetTitle());
      sp->SetDataWeightFast();
    }
    sp->SetPlot(kFALSE);
    sp->Fit();
    sp->sPlot();

    if(lastWeights) {delete lastWeights;lastWeights=nullptr;}
    lastWeights=sp->GetWeights();

  }
  //Now final fit for parameters
  //load new data
  DS->Print();
  fHSRooFit->LoadDataSet(DS);
    //do splot fit
  if(lastWeights) fHSRooFit->LoadWeights(lastWeights);
  fHSRooFit->SetWeightName(sp->GetTitle());
  fHSRooFit->SetDataWeightFast();
  fHSRooFit->Fit();
  
  // //plot if requested, this takes time so generally have off
  // if(fIsPlot) fHSRooFit->PlotDataModel();
  
  RooFitResult* fr=(RooFitResult*)fHSRooFit->GetResult()->clone();
  if (fr->status()==0) {
    _ngenVar->setVal(DS->sumEntries()) ;
    _nllVar->setVal(fr->minNll()) ;
    storeSummaryOutput(*_params) ;
    storeDetailedOutput(*fr) ;
  }
  
  
  //fHSRooFit->SetTitle(TString(GetTitle())+Form("_Study_%d_",summaryData()->numEntries()));
  // if(fIsPlot){fHSRooFit->SavePlots("");fHSRooFit->GetPlots()->Clear();}
  //fHSRooFit->GetPlots()->Clear();
  for(Int_t isp=0;isp<fsPlots->GetEntries();isp++){
    //cleanup previous fit
    sp->DeleteWeights();
    sp= dynamic_cast<THSsPlot*>(fsPlots->At(isp));
    sp->RemoveDataSet();
    sp->CleanWSDataSets();
  }
  fHSRooFit->RemoveDataSet();
  fHSRooFit->CleanWSDataSets();
 
  
  return kFALSE ;
} 



//_____________________________________________________________________________
Bool_t RooHSsPlotAndFitStudy::finalize() 
{

  fWS->loadSnapshot("initial");//load initial back parameters

  // Finalization of study
  delete _params ;
  delete _nllVar ;
  delete _ngenVar ;
  delete _initParams ;
  //delete _genSpec ;
  _params = 0 ;
  _nllVar = 0 ;
  _ngenVar = 0 ;
  _initParams = 0 ;
  // _genSpec = 0 ;
  return kFALSE ; 


} 


//_____________________________________________________________________________
void RooHSsPlotAndFitStudy::Print(Option_t* /*options*/) const
{
 }
Bool_t RooHSsPlotAndFitStudy::terminate()
{
  //this is done after finalize() to allow proof data to be merged
  //Make plots of parameter distributions and save
  fWS=fHSRooFit->GetWorkSpace();
  fWS->loadSnapshot("initial");//load initial parameters
  TFile outfile(fOutDir+"/Study"+fHSRooFit->GetName()+".root","recreate");
  for(Int_t iv=0;iv<fHSRooFit->GetParameters().getSize();iv++){
    RooRealVar* var=(fWS->var(fHSRooFit->GetParameters()[iv].GetName()));
    RooRealVar m("m","m",0,var->getMin(),var->getMax()) ;
    RooRealVar s("s","s",(var->getMax()-var->getMin())/10,0,(var->getMax()-var->getMin())*10);
    RooGaussian g("g","g",*var,m,s) ;
    g.fitTo(*summaryData());
    RooPlot *frame=var->frame();
    summaryData()->plotOn(frame);
    g.paramOn(frame,Layout(0.1, 0.4, 0.9),
	      Format("NEU",AutoPrecision(2)),
	      ShowConstants());
    g.plotOn(frame);
    //now pulls
     RooRealVar* genPar =(RooRealVar*) ((RooRealVar*)fGenRooFit->GetWorkSpace()->var(var->GetName()))->Clone(TString(var->GetName())+"truth");
    RooPullVar pull(TString(var->GetName())+"Pull",TString(var->GetName())+" Pull",*var,*genPar);
    summaryData()->addColumn(pull,kFALSE) ;
    fWS->import(pull);
    
    RooRealVar pvar(TString(var->GetName())+"Pull",TString(var->GetName())+" Pull",-5,5) ;
    RooPlot *framePull=pvar.frame();
    summaryData()->plotOn(framePull);
    
    RooRealVar mp("mp","mp",0,-5,5) ;
    RooRealVar sp("sp","sp",1,0,10);
    RooGaussian gp("gp","gp",pvar,mp,sp);
    gp.fitTo(*summaryData());
    gp.paramOn(framePull,Layout(0.1, 0.4, 0.9),
	       Format("NEU",AutoPrecision(2)),
	       ShowConstants());
    gp.plotOn(framePull);

    frame->Write();
    framePull->Write();
   }
  summaryData()->Write();
  TTree*  treeSum=RooStats::GetAsTTree("studyTree","studyTree",*summaryData());
  treeSum->Write();
  outfile.Close();


  
  return kFALSE;
}


