#include "RooHSAbsEventsPDF.h"
#include "Riostream.h" 
#include "RooRealVar.h" 
#include "RooRandom.h" 
#include "RooDataSet.h" 
#include "RooMsgService.h" 
#include "RooGaussian.h" 
#include "RooPlot.h" 
#include "RooPullVar.h" 
#include "TMath.h" 
#include "TCanvas.h" 

ClassImp(RooHSAbsEventsPDF)



RooHSAbsEventsPDF::RooHSAbsEventsPDF(const RooHSAbsEventsPDF& other, const char* name) :  RooAbsPdf(other,name)
  {
    fIsClone=kTRUE;
    fParent=const_cast<RooHSAbsEventsPDF*>(&other);
    if(other.fEvTree)fEvTree=(TChain*)other.fEvTree->Clone();
    fNInt=other.fNInt;
    fGeni=other.fGeni;
    if(other.fEntryList)fEntryList=(TEntryList*)other.fEntryList->Clone();
    fForceConstInt=other.fForceConstInt;
    fConstInt=other.fConstInt;
    fUseWeightsGen=other.fUseWeightsGen;
  }
RooHSAbsEventsPDF::~RooHSAbsEventsPDF(){
  //RooFit clones everything so I need to give the original
    //object the entrylist if I want to use it!
    if(fIsClone&&fParent&&fEntryList){
      if(fMaxValue){//has this clone used generator?
	fParent->SetEntryList(fEntryList);
	fParent->SetGeni(fGeni);
      }
    }
    if(fEntryList) delete fEntryList;
    if(fLast) delete fLast;
    if(fEvTree) delete fEvTree;
    
    if(fWeights){
      fWeights->Save();
      delete fWeights;
    }
    for(UInt_t i=0;i<fVarSet.size();i++)
      delete fVarSet[i];
    
    fVarSet.clear();
  }

void RooHSAbsEventsPDF::InitSets(){
  //Should be called at end of derived constructor
  // RooArgSet* aset0=new RooArgSet(Form("VarSet_All"));
  // for(UInt_t i=0;i<fProxSet.size();i++){
  //   //fVarSet.at(0)->add((fProxSet.at(i)->arg()));
  //   aset0->add(fProxSet.at(i)->arg());
  // }
  // fVarSet.push_back(aset0);
  // for(UInt_t i=0;i<fProxSet.size();i++){// create var sets without fProxSet[i]
  //   RooArgSet* aset=new RooArgSet(Form("VarSet_%d",i));
  //   for(UInt_t j=0;j<fProxSet.size();j++)//add if not proxy being removed
  //     if(fProxSet[i]->GetName()!=fProxSet[j]->GetName())aset->add(fProxSet.at(j)->arg());
    
  //   fVarSet.push_back(aset);
  // }
  fNpars=fParSet.size();
  fNvars=fProxSet.size();
  fLast=new Double_t[fNpars+1]; //Number of fit parameters
  for(Int_t i=0;i<fNpars+1;i++)
    fLast[i]=100;
}
RooArgSet RooHSAbsEventsPDF::VarSet(Int_t iset) const{
  RooArgSet aset(Form("VarSet_%d",iset));
  if(iset==0){
    for(UInt_t i=0;i<fProxSet.size();i++){
      aset.add(fProxSet.at(i)->arg());
    }
  }
  else{
    for(UInt_t j=0;j<fProxSet.size();j++){//add if not proxy being removed
      fProxSet.at(j)->arg().Print("");
      //if(fProxSet[iset]->GetName()!=fProxSet[j]->GetName()) aset.add(fProxSet.at(j)->arg());
      if(fProxSet[iset-1]->GetName()!=fProxSet[j]->GetName()) aset.add(fProxSet.at(j)->arg());
    }
  }
  return aset;
}
  Int_t RooHSAbsEventsPDF::getGenerator(const RooArgSet& directVars, RooArgSet &generateVars, Bool_t staticInitOK) const
{
  Info("RooHSAbsEventsPDF::getGenerator","Looking for generator");
  if(!fEvTree) return 0; //no MC events to generate from
  //case generate all variables
  //if (matchArgs(directVars,generateVars,*fVarSet[0])) return 1 ;
  if (matchArgs(directVars,generateVars,VarSet(0))) return 1 ;
  return 0;

}
void RooHSAbsEventsPDF::initIntegrator()
{
   //First check if gen branches are available and use them
  fEvTree->ResetBranchAddresses();
  for(UInt_t i=0;i<fProxSet.size();i++){
    //Look for true branches
    if(fEvTree->GetBranch(fProxSet[i]->GetName())){
      fEvTree->SetBranchStatus(fProxSet[i]->GetName(),1);
      fEvTree->SetBranchAddress(fProxSet[i]->GetName(),&fMCVar[i]);
     }
  }
}
void RooHSAbsEventsPDF::initGenerator(Int_t code)
{
  Info("RooHSAbsEventsPDF::initGenerator","Going to generate starting from %lld with weights %d",fGeni,(Int_t)fUseWeightsGen);
  //First check if gen branches are available and use them
   if(fEvTree->GetBranch(fgenStr+fProxSet[0]->GetName()))fEvTree->ResetBranchAddresses();

  for(UInt_t i=0;i<fProxSet.size();i++){
    //Look for true branches
    if(fEvTree->GetBranch(fgenStr+fProxSet[i]->GetName())){
      fEvTree->ResetBranchAddress(fEvTree->GetBranch(fProxSet[i]->GetName()));
      fEvTree->SetBranchStatus(fgenStr+fProxSet[i]->GetName(),1);
      fEvTree->SetBranchAddress(fgenStr+fProxSet[i]->GetName(),&fMCVar[i]);
    }
    else{
      Warning("RooHSAbsEventsPDF::initGenerator"," No gen branches found will use standard");
    }
  }
  Double_t value=0;
  if(code==1){
    //Brute force find maximum value
    //for(Int_t i=0;i<fEvTree->GetEntries();i++){
    for(Int_t i=1;i<fEvTree->GetEntries();i++){
      fEvTree->GetEntry(i);
        value=evaluateMC();
      if(value>fMaxValue)fMaxValue=value;
    }
  }
  //construct entry list so can reproduce full tree branches,
  //not jist those loaded as variables
  if(!fEntryList){
    fEntryList=new TEntryList("GenEvents","GenEvents",fEvTree);
  }
  else{
    fEntryList->Reset();
    fEntryList->SetTree(fEvTree);
  }
  if(fUseWeightsGen){
    fWeights=new THSWeights("genWeights");
    fWeights->SetSpecies(GetName());
    fWeights->SetFile(TString(GetName())+"Weights.root");
  }
  Info("RooHSAbsEventsPDF::initGenerator","Max value %lf",fMaxValue);
  cout<<fEvTree->GetEntries()<<endl;

 }
void RooHSAbsEventsPDF::generateEvent(Int_t code){
  // Info("RooHSAbsEventsPDF::generateEvent","Going to generate starting from %ld with ",fGeni);
  
  Double_t value=0;
  if(!fUseWeightsGen){
    while(fGeni<fEvTree->GetEntries()){
      fEvTree->GetEntry(fGeni++);
      value=evaluateMC();
      if(value>fMaxValue*RooRandom::uniform()){//accept
	for(Int_t i=0;i<fNvars;i++)
	  (*(fProxSet[i]))=fMCVar[i];
	fEntryList->Enter(fGeni-1);
	return;
      }
    }
  }
  else{
    //using weights
    while(fGeni<fEvTree->GetEntries()){
      fEvTree->GetEntry(fGeni++);
      if(!CheckRange("")) continue;
      value=evaluateMC();
      for(Int_t i=0;i<fNvars;i++)
	(*(fProxSet[i]))=fMCVar[i];
      fWeights->FillWeight(fGeni-1,value); 
      fEntryList->Enter(fGeni-1);
      return;
    }
  }
  Error("RooHSAbsEventsPDF::generateEvent","Ran out of events at %lld",fGeni);
  //Used up all the events in the tree!
}
Int_t RooHSAbsEventsPDF::getAnalyticalIntegral(RooArgSet& allVars, RooArgSet& analVars,const char* rangeName) const
{ //return 0; //might be good to check numerical integral sometimes

  if(!fEvTree) return 0; //no MC events to integrate over
  if(fProxSet.size()==1){//special case 1 variable
    //  fVarSet[0]->Print("");
    //    if (matchArgs(allVars,analVars,*fVarSet[0])) return 1 ;
    if (matchArgs(allVars,analVars,VarSet(0))) return 1 ;
  }
  else{
    //    for(UInt_t i=0;i<1+fProxSet.size();i++){
    for(UInt_t i=0;i<1+fProxSet.size();i++){
      //fVarSet[i]->Print("");
      //if (matchArgs(allVars,analVars,*fVarSet[i])) return i+1 ;
      if (matchArgs(allVars,analVars,VarSet(i))) return i+1 ;
      
    }
  }
    return 0;
}

Double_t RooHSAbsEventsPDF::analyticalIntegral(Int_t code,const char* rangeName) const
{
  //sort number of events first in case forced
  Long64_t NEv=0;
  if(fNInt>-1) NEv=fNInt;
  else NEv=fEvTree->GetEntries(); 
  if(NEv>fEvTree->GetEntries()) NEv=fEvTree->GetEntries();

  // Info("RooHSAbsEventsPDF::analyticalIntegral","calcing my own integral");
  // return 1;
  //only recalculate if a par changes when all variables included (ie code=1)
  if(code==1&&fForceConstInt) {fLast[0]=fConstInt/NEv;return fLast[0];}
  if(code==1)
    if(!CheckChange()) return fLast[0];
  // if(fNInt)fEvTree->SetEntries(fNInt);
  //In case changed for generation
  
  Double_t integral=0;
 
  if(code==1){
    for(Int_t ie=0;ie<NEv;ie++){
      fEvTree->GetEntry(ie);
      if(!CheckRange(rangeName)) continue;
      integral+=evaluateMC();
    }
  }
  else {
    //inegrate over other variables for one variable fixed
    //index given by code -2 (defintion of code in getAnalyticalIntegral
    //This is used for plotting data and PDFS
    
    //cout<<fMCVar.size()<<" "<<fProxSet.size()<<" "<<*(fProxSet[code-2])<<endl;
    Double_t rmax=fProxSet[code-2]->max();
    Double_t rmin=fProxSet[code-2]->min();
    Double_t vval=*(fProxSet[code-2]);
    Double_t vrange=rmax-rmin;
    Int_t nbins=((RooRealVar*)(&(fProxSet[code-2]->arg())))->getBins();
    Double_t delta=vrange/nbins/2;
    for(Int_t ie=0;ie<NEv;ie++){
      fEvTree->GetEntry(ie);
      if(!CheckRange(rangeName)) continue;
     //only inlcude events within same bin as vval in integral
      if(TMath::Abs(fMCVar[code-2]-vval)>delta)continue;
      integral+=evaluateMC();
    }
    //correct for delta integration width
    //first 2 case near range limits 
    if((rmax-vval)<delta) delta=delta+rmax-vval;
    else if((vval-rmin)<delta) delta=delta+vval-rmin;
    else delta=delta*2;
    integral=integral/delta;
  }
  //else return 1;

  //Set Last[0] so we can just return that if no parameter changes
  if(fNMCGen) fLast[0]=integral/fNMCGen;
  else fLast[0]=integral/NEv;


  return fLast[0];
}

Bool_t RooHSAbsEventsPDF::CheckRange(const char* rangeName) const{
  for(UInt_t i=0;i<fProxSet.size();i++){
    RooRealVar* var=((RooRealVar*)(&(fProxSet[i]->arg())));
    if(!var->inRange(fMCVar.at(i),rangeName)) return kFALSE;
  }
  return kTRUE;

}
Bool_t RooHSAbsEventsPDF::CheckChange() const{
  //Note analytical integral is const funtion so can only change data members
  //which are pointed to something, thus need Double_t *fLast
  //and construct a N-D array where we can change elements

  Bool_t hasChanged=0;
  for(Int_t i=1;i<fNpars+1;i++)
    if(fLast[i]!=(*(fParSet[i-1]))) hasChanged=1;
  if(hasChanged){
    for(Int_t i=1;i<fNpars+1;i++)
      fLast[i]=*(fParSet[i-1]);
  }
  return hasChanged;
}
Bool_t RooHSAbsEventsPDF::SetEvTree(TChain* tree,Long64_t ngen){
  Info("RooHSAbsEventsPDF::SetEvTree"," with name %s",tree->GetName());
  fEvTree=tree;
  fNMCGen=ngen;
  fConstInt=fEvTree->GetEntries();
   // RooArgList set1=fVarSet.at(0);
  fMCVar.reserve(fProxSet.size());
  fEvTree->ResetBranchAddresses();
  fEvTree->SetBranchStatus("*",0);
  for(UInt_t i=0;i<fProxSet.size();i++){
    //cout<<fProxSet[i]->GetName()<<" "<<fMCVar.size()<<endl;
    Double_t tempD=0;
    fMCVar.push_back(tempD); //initalise element
    if(fEvTree->GetBranch(fProxSet[i]->GetName())){
      fEvTree->SetBranchStatus(fProxSet[i]->GetName(),1);
      fEvTree->SetBranchAddress(fProxSet[i]->GetName(),&fMCVar[i]);
      }
    else{
      Error("RooHSAbsEventsPDF::SetEvTree","Branch %s not found",fProxSet[i]->GetName());
      return kFALSE;
    }
  }
  // cout<<"CHECK TREE "<<endl;
  // fEvTree->GetEntry(1);
  // cout<<fMCVar[0]<<" "<<fMCVar[1]<<endl;
  return kTRUE;
}
void  RooHSAbsEventsPDF::CheckIntegralParDep(Int_t Ntests){
  Long64_t saveNint=fNInt;
  // fNInt=nint;//REOMVE FOR NOW nint is not used
  fNInt=fEvTree->GetEntries();//just use all entries
  //scale Ntests by Ndimensions
  Ntests=TMath::Power((Double_t)Ntests,(Double_t)fParSet.size());

  Info("RooHSAbsEventsPDF::CheckIntegralParDep","Going to run %d calculations of integral with random parameters",Ntests);
  
  RooRealVar integral("integral","integral",0,0,2);
  integral.setError(sqrt(fNInt)/fNInt); //Error needs to be set before entering in ds
 RooDataSet ds("intds","intds",RooArgSet(integral));
  //want to set random paramter values
  //loop over each paramter and calculate integral
 vector<Double_t> SavedPars;
 for(UInt_t ip=0;ip<fParSet.size();ip++){//first save parameters
   RooRealVar* par=((RooRealVar*)(&(fParSet[ip]->arg())));
   SavedPars.push_back(par->getValV());
 }
 for(Int_t ir=0;ir<Ntests;ir++){ //loop over tests
    for(UInt_t ip=0;ip<fParSet.size();ip++){//loop over parameters
      RooRealVar* par=((RooRealVar*)(&(fParSet[ip]->arg())));
      par->setVal((par->getMax("")-par->getMin(""))*RooRandom::uniform()+par->getMin(""));
    }
    //Now calculate integral
    integral.setVal(analyticalIntegral(1,""));
    ds.add(RooArgSet(integral));
  }

 
  Double_t low=0;
  Double_t high=0;
  ds.getRange(integral,low,high);
  integral.setRange(low,high);
  RooPlot *frame=integral.frame();
  ds.plotOn(frame);

  frame->Draw();

  RooRealVar  mean("mean","mean",ds.mean(integral));
  RooRealVar pvar("IntPull","Integral Pull Dist.",-5,5);
  RooPullVar pull("IntPull","Integral Pull Dist.",integral,mean);
  ds.addColumn(pull,kFALSE);

  ds.Print();
  ds.getRange(pvar,low,high);
  pvar.setRange(low,high);
  RooPlot *framePull=pvar.frame();
  ds.plotOn(framePull);
  RooRealVar mp("mp","mp",0,-5,5) ;
  RooRealVar sp("sp","sp",1,0,100);
  RooGaussian gp("gp","gp",pvar,mp,sp);
  gp.fitTo(ds);
  gp.paramOn(framePull);
  gp.plotOn(framePull);
  
  pvar.Print();
  new TCanvas();
  framePull->Draw();
  
 
  //numerical check gives constant integral, can force const for fit speed
  fConstInt=mean.getVal();
  fNInt=saveNint;
  if(sp.getVal()<1){
    Info("RooHSAbsEventsPDF::CheckIntegralParDep","Numerical integral constant. Will not recalculate during fits to save time, if you want to force calculation do not call this function");
    SetConstInt();
  }
  //reset pars
  for(UInt_t ip=0;ip<fParSet.size();ip++){//first save parameters
    RooRealVar* par=((RooRealVar*)(&(fParSet[ip]->arg())));
    par->setVal(SavedPars[ip]);
  }

}
