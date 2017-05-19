

#include "RooFit.h"
#include "Riostream.h"

#include "RooHS1StepStudy.h"
#include "RooHSAbsEventsPDF.h"
#include "RooRandom.h"
#include "RooWorkspace.h"
#include "RooMsgService.h" 
#include "RooDataSet.h"
#include "RooAbsPdf.h"
#include "RooRealVar.h"
#include "RooGlobalFunc.h"
#include "RooFitResult.h"
#include "RooPullVar.h"
  

using namespace std ;

ClassImp(RooHS1StepStudy)


//_____________________________________________________________________________
RooHS1StepStudy::RooHS1StepStudy(const char* name, const char* title) : 
  RooAbsStudy(name,title), fHSRooFit(0),fWS(0)
{  
  // Constructor
  fIsCloned=kFALSE;
}



//_____________________________________________________________________________
RooHS1StepStudy::RooHS1StepStudy(const RooHS1StepStudy& other) : 
  RooAbsStudy(other), fHSRooFit(0),fWS(0)
{ 
  fIsCloned=kTRUE;
  fHSRooFit=new THSRooFit((THSRooFit*)other.fHSRooFit);
  fWS=fHSRooFit->GetWorkSpace();
  fOutDir=other.fOutDir;
  fIsPlot=other.fIsPlot;
}



//_____________________________________________________________________________
RooHS1StepStudy::~RooHS1StepStudy()
{
  if(fIsCloned){
    if(fHSRooFit) {delete fHSRooFit;fHSRooFit=0;fWS=0;}
  }
}
Bool_t RooHS1StepStudy::attach(RooWorkspace& w) {
  fHSRooFit=new THSRooFit(TString(GetName()));
  fHSRooFit->LoadWorkSpace(&w);
  fHSRooFit->TotalPDF();
  fHSRooFit->SetOutDir(fOutDir);
  // fHSRooFit->SetName(TString(GetName())+Form("_Study_%d_",summaryData()->numEntries()));
  fWS=fHSRooFit->GetWorkSpace();
  if(!fWS){Error("RooHS1StepStudy::attach","No valid workspace"); return kFALSE;}
  return kTRUE;
}


//_____________________________________________________________________________
Bool_t RooHS1StepStudy::initialize() 
{ 
  // One-time initialization of study 

  _nllVar = new RooRealVar("NLL","-log(Likelihood)",0) ;
  _ngenVar = new RooRealVar("ngen","number of generated events",0) ;
  
  _params = new RooArgSet(fHSRooFit->GetParameters()) ;
  RooArgSet modelParams(*_params) ;
  fWS->saveSnapshot("initial",modelParams,kTRUE);
  _initParams = (RooArgSet*) _params->snapshot() ;
  _params->add(*_nllVar) ;
  _params->add(*_ngenVar) ;

  registerSummaryOutput(*_params,modelParams) ;
  
  // fHSRooFit->DefaultFitOptions();
  // fHSRooFit->AddFitOption(RooFit::Extended());
  // fHSRooFit->AddFitOption(RooFit::DataError(RooAbsData::SumW2));
  // fHSRooFit->AddFitOption(RooFit::SumW2Error(kTRUE));
  // fHSRooFit->AddFitOption(RooFit::Silence());
 
  return kFALSE ;
} 

//_____________________________________________________________________________
Bool_t RooHS1StepStudy::execute() 
{ 
  fHSRooFit->RemoveDataSet();
  fHSRooFit->CleanWSDataSets();

  //generate pseudo data set
  fWS->loadSnapshot("initial");//load initial parameters

  RooHSAbsEventsPDF* PDF=dynamic_cast<RooHSAbsEventsPDF*>(fWS->pdf(fPDFName));

  PDF->SetGeni(fGeni);
  //genereate n events from poisson
  Long64_t nexp=RooRandom::randomGenerator()->Poisson(fHSRooFit->GetModel()->expectedEvents(fHSRooFit->GetVariables()));
  //generate data from  model
  RooDataSet* DS=fHSRooFit->GetModel()->generate(fHSRooFit->GetVariables(),nexp);
  Info("RooHS1StepStudy::execute() ","generated pseudo data :");
  DS->Print();
  PDF->initIntegrator(); //move back to reconstruced branches
  //get generated tree, this will be flat
  TTree* tree=PDF->GetGenTree();
  fHSRooFit->LoadDataSet(tree);
  if(PDF->UseWeightsGen()){  //weight generated tree with model
    //If this is not set events will just be accept or reject
    fHSRooFit->LoadWeights(TString(fPDFName)+"Weights.root","genWeights");
    fHSRooFit->SetWeightName(fPDFName);
    fHSRooFit->SetDataWeightFast();
  }
  //Fit data
  fHSRooFit->Fit();

  //plot if requested, this takes time so generally have off
  if(fIsPlot) fHSRooFit->PlotDataModel();
  
   RooFitResult* fr=(RooFitResult*)fHSRooFit->GetResult()->clone();
  if (fr->status()==0) {
    _ngenVar->setVal(DS->sumEntries()) ;
    _nllVar->setVal(fr->minNll()) ;
    storeSummaryOutput(*_params) ;
    storeDetailedOutput(*fr) ;
  }
  
  fGeni=PDF->GetGeni();

  Info("RooHS1StepStudy::execute()","Done Got to event %lld in tree",fGeni);
  fHSRooFit->SetName(TString(GetName())+Form("_Study_%d_",summaryData()->numEntries()));
  if(fIsPlot)fHSRooFit->SavePlots("");
  delete DS ;
  return kFALSE ;
} 



//_____________________________________________________________________________
Bool_t RooHS1StepStudy::finalize() 
{

  //Make plots of parameter distributions and save
  RooArgSet corrPars("CorrectedPars");
  
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
    //    RooAbsReal* genPar =(RooAbsReal*) ((RooRealVar*)_initParams->find(var->GetName()))->Clone(TString(var->GetName())+"truth");
    RooRealVar* genPar =(RooRealVar*) ((RooRealVar*)_initParams->find(var->GetName()))->Clone(TString(var->GetName())+"truth");
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

    //    RooRealVar corPar(var->GetName(),var->GetTitle(),2*genPar->getValV()-m.getValV());
    RooRealVar corPar(var->GetName(),var->GetTitle(),genPar->getValV()-m.getValV());
    //approx error calculation
    Double_t err1=genPar->getError();
    Double_t err2=m.getError();
    // corPar.setError(sqrt(err1*err1+err2*err2));
    corPar.setError(sqrt(err2*err2));
    corrPars.addClone(corPar);
    
    frame->Write();
    framePull->Write();
    corPar.Print();
  }
  corrPars.Write();
  summaryData()->Write();

  outfile.Close();

  cout<<fIsPlot<<" "<<fOutDir<<endl;
  Info("RooHS1StepStudy::finalize()","Done Got to event %lld in tree",fGeni);

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
void RooHS1StepStudy::Print(Option_t* /*options*/) const
{
}


