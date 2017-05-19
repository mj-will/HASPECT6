#include "THSRooFit.h"
#include <TSystem.h>
#include <TRandom.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TDirectory.h>
#include <TLegend.h>
#include <TVectorD.h>
#include <RooGenericPdf.h>
#include <RooDataHist.h>
#include <RooAbsData.h>
#include <RooChi2Var.h>
#include <RooConstVar.h>
#include "RooHSAbsEventsPDF.h"
#include "RooHS1StepStudy.h"
#include <RooStudyManager.h>
#include <algorithm>      // std::sort
#include <list>     

ClassImp(THSRooFit);


THSRooFit::THSRooFit() : TNamed(), fWS(0), fID(0),fModel(0), fData(0), fCanvases(0),fHists(0),fTree(0),fMCIntTree(0),fMCGenTree(0),fResult(0),fRooFits(0),fInWeights(0),fDataBins(0),fBinnedFit(kFALSE),fFiti(0),fYld("Yld_"),fSingleSp(""){};

THSRooFit::THSRooFit(TString name) : TNamed(name,name), fWS(0),fID(0), fModel(0), fData(0), fCanvases(0),fHists(0),fTree(0),fMCIntTree(0),fMCGenTree(0),fResult(0),fRooFits(0),fInWeights(0),fDataBins(0),fBinnedFit(kFALSE),fFiti(0),fYld("Yld_"),fSingleSp(""){
 fWS = new RooWorkspace("wHS");
 fGotID=kFALSE;
 fCanvases=new TList();fCanvases->SetOwner();
 fHists=new TList();fHists->SetOwner();
 fRooFits=new TList();fRooFits->SetOwner();

}
THSRooFit::THSRooFit(THSRooFit* rf): fWS(0), fID(0),fModel(0), fData(0), fCanvases(0),fHists(0),fTree(0),fMCIntTree(0),fMCGenTree(0),fResult(0),fRooFits(0),fInWeights(0),fDataBins(0),fBinnedFit(kFALSE),fFiti(0),fYld("Yld_"),fSingleSp(""){
  //  THSRooFit();
   //copy constructor, but do not copy the data tree, load that explicitly
  if(rf->GetWorkSpace()){fWS=(RooWorkspace*)rf->GetWorkSpace()->Clone();}
  if(rf->GetModel())fModel=fWS->pdf(rf->GetModel()->GetName());
  for(Int_t i=0;i<rf->GetVariables().getSize();i++){
    if((fWS->var(rf->GetVariables()[i].GetName())))
      fVariables.add(*(fWS->var(rf->GetVariables()[i].GetName())));
    else if((fWS->cat(rf->GetVariables()[i].GetName())))
      fVariables.add(*((RooAbsArg*)fWS->cat(rf->GetVariables()[i].GetName())));
  }
  for(Int_t i=0;i<rf->GetAuxVars().getSize();i++){
    if((fWS->var(rf->GetAuxVars()[i].GetName())))
      fAuxVars.add(*(fWS->var(rf->GetAuxVars()[i].GetName())));
    else if((fWS->cat(rf->GetAuxVars()[i].GetName())))
      fAuxVars.add(*((RooAbsArg*)fWS->cat(rf->GetAuxVars()[i].GetName())));
  }
  for(Int_t i=0;i<rf->GetBinVars().getSize();i++){
    if((fWS->var(rf->GetBinVars()[i].GetName())))
      fBinVars.add(*(fWS->var(rf->GetBinVars()[i].GetName())));
    else if((fWS->cat(rf->GetBinVars()[i].GetName())))
      fBinVars.add(*((RooAbsArg*)fWS->cat(rf->GetBinVars()[i].GetName())));
  }
  for(Int_t i=0;i<rf->GetParameters().getSize();i++)
    fParameters.add(*(fWS->var(rf->GetParameters()[i].GetName())));
  for(Int_t i=0;i<rf->GetYields().getSize();i++)
    fYields.add(*(fWS->var(rf->GetYields()[i].GetName())));
  for(Int_t i=0;i<rf->GetPDFs().getSize();i++)
    fPDFs.add(*(fWS->var(rf->GetPDFs()[i].GetName())));
  for(Int_t i=0;i<rf->GetConstraints().getSize();i++)
    fConstraints.add(*(fWS->var(rf->GetConstraints()[i].GetName())));
  for(Int_t i=0;i<rf->GetFitOptions().GetSize();i++)
    fFitOptions.Add(rf->GetFitOptions().At(i));
  //fFitOptions=rf->fFitOptions.Clone();
  if(rf->GetBins())fDataBins=(THSBins*)rf->GetBins()->Clone();
  if(rf->fID) fID=rf->fID;
  fBinnedFit=rf->fBinnedFit;
  fSingleSp=rf->fSingleSp;
  fOutDir=rf->fOutDir;
  fBinDir=rf->fBinDir;
  fStudyPDF=rf->fStudyPDF;
  fNStudyTrials=rf->fNStudyTrials;
  fInWeights=rf->fInWeights;
  fWeightName=rf->fWeightName;
  }

THSRooFit::~THSRooFit(){

  if(fWS) delete fWS;
  //if(fCanvases)delete fCanvases;
  if(fHists)delete fHists;
  if(fResult) delete fResult;
  //if(fRooFits) delete fRooFits;
  if(fDataBins) delete fDataBins;
  if(fMCIntTree) delete fMCIntTree;
  if(fMCGenTree) delete fMCGenTree;
  // if(fID) delete fID;
}

void THSRooFit::LoadDataSet(TTree* tree,Bool_t toWS){
  ftoWS=toWS;
  //Take a tree and convert to RooDataSet
  if(!tree) {cout<<"Tree does not exist "<<endl;exit(0);}
  fTree=tree;
  RooArgSet dataVars(fVariables,fAuxVars);
  if(fID) dataVars.add(*fID);
  fData=new RooDataSet(tree->GetName(),tree->GetTitle(),dataVars,RooFit::Import(*tree)); //make roodatset if not splitting onto bins
  if(toWS) fWS->import(*fData); //import if told to
  else if(!(fDataBins))if(!fInWeights)fWS->import(*fData); //if not told to import if not bins
  //else if not told to import and there are bins don't import
  cout<<"THSRooFit::LoadDataSet(TTree*) Print dataset for "<<tree->GetName()<<endl;
  fData->Print();
}
void THSRooFit::RemoveDataSet(){
  if(fData) {fData->SetName("Empty");fData->reset();}
  if(!fTree)return;//no tree dataset
  delete fTree;
  //if(fData) delete fData;//crashes workspace...
}
void THSRooFit::CleanWSDataSets(){
  list<RooAbsData*> datalist=fWS->allData();
  for (std::list<RooAbsData*>::iterator it = datalist.begin(); it != datalist.end(); it++){
    ((RooDataSet*)*it)->SetName("Cleared");
    ((RooDataSet*)*it)->reset();
  }
}
void THSRooFit::LoadDataSet(RooAbsData* data,Bool_t toWS){
  ftoWS=toWS;
  if(!data) {cout<<"LoadDataSet(RooAbsData*) No valid data set "<<endl;return;}
  //Take a tree and convert to RooDataSet
  //if(fWS->pdf("TotalPDF"))fWS->pdf("TotalPDF")->SetNameTitle("OldPDF","OldPDF");
  if(toWS) fWS->import(*data);
  else if(!(fDataBins))if(!fInWeights)fWS->import(*data); //if not told to import if not bins
  //else if not told to import and there are bins don't import
  fData=data;
  cout<<"THSRooFit::LoadDataSet Print dataset for "<<fData->GetName()<<endl;
  fData->Print();
}

void THSRooFit::LoadWorkSpaceData(RooWorkspace* ws){
  LoadWorkSpace(ws);
  LoadDataSet(ws->allData().front());//assumes only 1 data set!!
}
void THSRooFit::LoadWorkSpace(RooWorkspace* ws){
  //load model from workspace, does not load data
  //This allows to create a new THSRooFit object for different trees
  if(!ws) {cout<<"Warning void THSRooFit::LoadWorkSpace NULL workspace supplied returning"<<endl; return;}
    if(fWS) delete fWS;
  fWS=new RooWorkspace(*ws);
  if(fWS->pdf("TotalPDF0"))fModel=fWS->pdf("TotalPDF0");
  if(fWS->set("Variables")){fVariables.add(*(fWS->set("Variables")));fWS->import(fVariables);}
  if(fWS->set("AuxVars"))fAuxVars.add(*(fWS->set("AuxVars")));
  if(fWS->set("BinVars"))fBinVars.add(*(fWS->set("BinVars")));
  if(fWS->set("Parameters"))fParameters.add(*(fWS->set("Parameters")));
  if(fWS->set("Yields"))fYields.add(*(fWS->set("Yields")));
  if(fWS->set("PDFs"))fPDFs.add(*(fWS->set("PDFs")));
  if(fWS->set("Constraints"))fConstraints.add(*(fWS->set("Constraints")));
  // if(fWS->set("FitOptions"))fFitOptions.add(*(fWS->set("FitOptions")));
  if(fWS->set("ID")) {
    fIDBranchName=(fWS->set("ID"))->first()->GetName();
    fID=fWS->var(fIDBranchName);
    fGotID=kTRUE;
  }
  else fGotID=kFALSE;
  if(!fWS->set("PDFs"))DefineSets();
  // fWS->Print();
}
void THSRooFit::DefineSets(){	
  //define var types in workspace, for easy reloading
  fWS->defineSet("Variables",fVariables);
  fWS->defineSet("AuxVars",fAuxVars);
  fWS->defineSet("BinVars",fBinVars);
  fWS->defineSet("Parameters",fParameters);
  fWS->defineSet("Yields",fYields);
  fWS->defineSet("PDFs",fPDFs);
  fWS->defineSet("Constraints",fConstraints);
  // fWS->defineSet("FitOptions",fFitOptions);
}
void THSRooFit::LoadWeights(TString wfile,TString wname){
  //GetWeights object 
  cout<<"void THSRooFit::LoadWeights "<<wfile<<" "<<wname<<endl;
  if(fInWeights) delete fInWeights;
  fInWeights=0;
  fInWeights=new THSWeights();
  fInWeights->LoadSaved(wfile,wname);
  
  fInWeights->PrintWeight();
}
void THSRooFit::SetDataWeight(){
  if(!fInWeights) return;
  if(fInWeights->GetSpeciesID(fWeightName)<0) return;
  RooArgList setWeights;
  RooRealVar wvar(fWeightName,fWeightName,0);
  setWeights.add(wvar);
  setWeights.Print();
  RooDataSet wData("weights","weights",setWeights) ;
  cout<<"Print data "<<endl;
 fData->Print();
  //Loop over the weights find the entry for the event ID
  for(Int_t ev=0;ev<fData->numEntries();ev++){
    const RooArgSet* vars=fData->get(ev);
    fInWeights->GetEntryBinarySearch((Long64_t)vars->getRealValue(fIDBranchName));
    // wvar.setVal(fInWeights->GetWeight(fWeightName));
    wvar.setVal(fInWeights->GetWeight(fWeightName));
    wData.add(setWeights);
  }
  wData.Print();
  ((RooDataSet*)fData)->merge(&wData);
  fData->Print();
  RooDataSet* tmpData=0;
  tmpData=new RooDataSet(TString("Weight_")+fWeightName+fData->GetName(),fData->GetTitle(),(RooDataSet*)fData,*fData->get(),0,fWeightName);
  fData->SetName("Empty");fData->reset();fData=0;
  fData=tmpData;
  fData->Print();
  if(ftoWS) fWS->import(*fData); //import if told to
  else if(!(fDataBins))fWS->import(*fData); //or no databins to confuse it 
}
void THSRooFit::SetDataWeightFast(){
  //Ignore ID just merge with dataset which should have same entries
  if(!fInWeights) return;
  if(fInWeights->GetSpeciesID(fWeightName)<0) return;
  if(fInWeights->GetTree()->GetEntries()!=fData->numEntries()) {
    Error("THSRooFit::SetDataWeightFast()"," dataset and weights do not match entries");
    return;
  }
  RooArgList setWeights;
  RooRealVar wvar(fWeightName,fWeightName,0);
  setWeights.add(wvar);
  setWeights.Print();

  RooDataSet wData("Weights","Weights",setWeights,RooFit::Import(*fInWeights->GetTree())); //make roodatset if not splitting onto bins
  wData.Print();
  ((RooDataSet*)fData)->merge(&wData);
  
  fData->Print();
  RooDataSet* tmpData=0;
  tmpData=new RooDataSet(TString("Weight_")+fWeightName+fData->GetName(),fData->GetTitle(),(RooDataSet*)fData,*fData->get(),0,fWeightName);
  fData->SetName("Empty");fData->reset();fData=0;
  fData=tmpData;
  fData->Print();
  if(ftoWS) fWS->import(*fData); //import if told to
  else if(!(fDataBins))fWS->import(*fData); //or no databins to confuse it 
}
void THSRooFit::SetWeightName(TString WName){
  if(!fInWeights) return;
  if(fInWeights->GetSpeciesID(WName)<0) return;
  fWeightName=WName;
}
void THSRooFit::LoadVariable(TString opt){
  fVariables.add(*(fWS->factory(opt)));
}
// void THSRooFit::LoadCategory(TString opt){
//   fVariables.add(*(fWS->factory(opt)));
// }
void THSRooFit::LoadAuxVars(TString opt){
  fAuxVars.add(*(fWS->factory(opt)));
}
void THSRooFit::LoadBinVars(TString opt,Int_t nbins,Double_t min,Double_t max){
  if(!fDataBins) fDataBins=new THSBins("HSDataBins");
  //Load a variable that is to be used to define the kinmatic bins
  //opt must be the name of a variable in the tree
  // LoadAuxVars(opt+Form("[%lf,%lf]",min,max));//dummy range, needed for factory to create var
  fBinVars.add(*(fWS->factory(opt+Form("[%lf,%lf]",min,max))));
  fDataBins->AddAxis(opt,nbins,min,max);

}
void THSRooFit::LoadBinVars(TString opt,Int_t nbins,Double_t* xbins){
  if(!fDataBins) fDataBins=new THSBins("HSDataBins");
  //Load a variable that is to be used to define the kinmatic bins
  //opt must be the name of a variable in the tree
  // LoadAuxVars(opt+Form("[%lf,%lf]",min,max));//dummy range, needed for factory to create var
  fBinVars.add(*(fWS->factory(opt+Form("[%lf,%lf]",xbins[0],xbins[nbins-1]))));
  fDataBins->AddAxis(opt,nbins,xbins);

}
RooRealVar* THSRooFit::GetVar(TString name){
  RooRealVar* var=0;
  if(var==GetVariable(name)) return var;
  else if(var==GetAuxVar(name)) return var;
  else cout<<"THSRooFit::GetVar no variable with name "<<name<<endl;
  return nullptr;
}
void THSRooFit::LoadSpeciesPDF(TString opt,Int_t Scale0){
  //Scale gives relative starting value to other species
  //Get PDF from workspace and add to PDF list
  fPDFs.add(*((RooGenericPdf*)fWS->pdf(opt)));//RooGeneric is just a dummy, add does not take RooAbsPdf
  //fPDFs.find(opt);
  fParameters.add(*(fPDFs.find(opt)->getParameters(RooArgSet(fVariables,fAuxVars))));// get parameters not in fVariables 
  //Add a yield parameter for this species
  fYields.add(*(fWS->factory(fYld+opt+Form("[%d,0,1E12]",Scale0))));//default yields limits
}
void THSRooFit::TotalPDF(){
  //Construct a total PDF whcih is the sum of the species PDFs
  RooAddPdf model("TotalPDF","total model",
		  fPDFs, 
		  fYields);
  Int_t Nm=0;
  //can't delete from workspace!
  while(fWS->pdf(Form("TotalPDF%d",Nm++)));
  model.SetName(Form("TotalPDF%d",Nm-1));
    
  fWS->import(model); //and replace any existing model memory leak?
  fModel=fWS->pdf(model.GetName());
  fModel->Print();
  fYields[0].Print();
  AddFitOption(RooFit::Extended());
}
void THSRooFit::Fit(Bool_t randPar){   
  Info("THSRooFit::Fit()"," Starting");
  if(!fData){
    Error("THSRooFit::Fit"," no dataset loaded");
    return;
  }
  fModel->Print();
  if(randPar) RandomisePars();
  if (fResult) {delete fResult;fResult=0;}
  //Initialise yield fit parameters
  for(Int_t iy=0;iy<fYields.getSize();iy++){//set yields below entries in dataset
    if(fData->sumEntries()<=1){
      cout<<"void THSRooFit::Fit SUMENTRIES "<<fData->sumEntries()<<endl;;
      fWS->var(fYields[iy].GetName())->setRange(0,fData->numEntries()); 
      // fWS->var(fYields[iy].GetName())->setVal(1E3/2/fWS->var(fYields[iy].GetName())->getVal());//scale by value give in LoadSpecies 
      fWS->var(fYields[iy].GetName())->setVal(fData->numEntries()/2);//scale by value give in LoadSpecies 

    }
    //if(fFiti==0){
      fWS->var(fYields[iy].GetName())->setRange(0,fData->sumEntries()*2); 
      //fWS->var(fYields[iy].GetName())->setVal(fData->sumEntries()/2/fWS->var(fYields[iy].GetName())->getVal());//scale by value give in LoadSpecies
      fWS->var(fYields[iy].GetName())->setVal(fData->sumEntries()/2);//scale by value give in LoadSpecies
      //}
  }
  if(fFiti==0){
    DefaultFitOptions();
    //Add any constraints to the likelihood
    fConstraints.Print();
    if(fConstraints.getSize()) AddFitOption(RooFit::ExternalConstraints(fConstraints));
  }
    fFitOptions.Print("");
  if(fBinnedFit){
    if(fConstraints.getSize())cout<<"Warning : THSRooFit::Fit Constraints defined for binned fit, this is will not work in RooFit has to be ML fit"<<endl;  
    RooRealVar *var=((RooRealVar*)&fVariables[0]);//assume fitting first variable
    Double_t hmin;
    Double_t hmax;
    fData->getRange(*var,hmin,hmax);
    // var->setRange("R1",hmin,hmax);
    //TH1D* hist=(TH1D*)fData->fillHistogram(new TH1D("binned","binned",var->getBins(),var->getMin(""),var->getMax("")),RooArgList(*var));
    TH1D* hist=(TH1D*)fData->fillHistogram(new TH1D("binned","binned",var->getBins(),hmin,hmax),RooArgList(*var));
    if(hist->GetMinimum()<10) hist->Rebin();
    Bool_t ForceMLL=kFALSE;
    if(hist->GetMinimum()<5){cout<<"THSRooFit::Fit binned fit requested but some bins have few entries, switching to likelihood "<<endl;ForceMLL=kTRUE;}
    hist->Sumw2();
    RooDataHist rdh(Form("chi2hist%d",fFiti),Form("chi2hist%d",fFiti),RooArgList(*var),RooFit::Import(*hist));
    cout<<"MAX MIN VALUES "<<hmin<<" "<<hmax<<endl;
    RooCmdArg dataerr=RooFit::DataError(RooAbsData::SumW2);
    fFitOptions.Print("");
    if(!fFitOptions.FindObject(&dataerr)) fFitOptions.Add(&dataerr);

    if(!ForceMLL)fResult=fModel->chi2FitTo(rdh,fFitOptions);
    else fResult=fModel->fitTo(rdh,fFitOptions);
    
    fFitOptions.Remove(&dataerr);
    delete hist;
    
  }
  else
    fResult=fModel->fitTo(*fData,fFitOptions);
  
  fFiti++;

}
void THSRooFit::RandomisePars(){
  //randomise fit parameters
  for(Int_t ip=0;ip<fParameters.getSize();ip++){
    RooRealVar *par=((RooRealVar*)&fParameters[ip]);
    //check if par this is fxed constant.
    if(par->isConstant()) continue;
    //Look through constraints to see if one is defined for this parameter
    Bool_t hadCon=kFALSE;
    for(Int_t ic=0;ic<fConstraints.getSize();ic++){
      RooAbsPdf *pdfCon=((RooAbsPdf*)&fConstraints[0]);//get RooPdf constraint
      if(pdfCon->getObservables(fParameters)->contains(*par)){ //does it contain par?
	//Yes, must generate random number from constraint
	RooArgSet setPar(*par); //make an argset from this 1 par as needed for..
	RooDataSet *oneEv=pdfCon->generate(setPar,1); //gen 1 event
	const RooArgSet* theEv = oneEv->get(); //get the event
	theEv->getRealValue(par->GetName()); //get par value of event
	hadCon=kTRUE;
	delete oneEv;
	break;//can only have 1!	
      }
    }
    //If there was no constraint to select from just take random in range 
    if(!hadCon)par->setVal(gRandom->Uniform(par->getMin(""),par->getMax("")));
  }
}

void THSRooFit::PlotModel(TString varName,TString pdfName){
  TCanvas *canvas=0;
  if(!fCanvases){fCanvases=new TList();fCanvases->SetOwner();}
  RooRealVar* var=fWS->var(varName);//get variable
  fCanvases->Add(canvas=new TCanvas(TString(GetName())+varName+pdfName,TString(GetName())+varName+pdfName));//create new canvas for drawing on
  RooPlot* frame = var->frame(); // RooFit frame
  fWS->pdf(pdfName)->plotOn(frame,LineStyle(kSolid), LineColor(kRed));
  frame->SetTitle(pdfName+TString(" versus ")+varName);
  frame->Draw();
  canvas->Modified();
  canvas->Update();

}
void THSRooFit::PlotDataModel(){
  //Function to plot the data and fitted model for each variable

  TCanvas *canvas=0;
  if(!fCanvases){fCanvases=new TList();fCanvases->SetOwner();fCanvases->SetName(TString("RFPlots")+GetName());}
  //Loop over variables
  for(Int_t idr=0;idr<fVariables.getSize();idr++){
    cout<<"Plotting versus "<<fVariables[idr].GetName()<<endl;
    RooRealVar* var=fWS->var(fVariables[idr].GetName());//get variable
    
    if(!var) continue;
    fCanvases->Add(canvas=new TCanvas(TString(GetName())+fVariables[idr].GetName()+Form("%d",fFiti),TString(GetName())+fVariables[idr].GetName()));//create new canvas for drawing on
    RooPlot* frame = var->frame(); // RooFit frame 
    fData->plotOn(frame, DataError(RooAbsData::SumW2) ) ; //plot the data
    fModel->plotOn(frame,LineColor(kRed),Precision(1E-2)) ; //model = signal + back fit result 
    //Get the chi2
    fChi2=frame->chiSquare();
    cout<<fFiti<<" THSRooFit::PlotDataModel() chi2 = "<<fChi2<<endl;
    fModel->paramOn(frame,
		    Layout(0.1, 0.4, 0.9),
		    Format("NEU",AutoPrecision(2)),
		    ShowConstants()); //show fit parameters
    frame->SetTitle(TString("Fit components for ")+fVariables[idr].GetName());
    //Loop over components
    for(Int_t ic=0;ic<fPDFs.getSize();ic++)
      fModel->plotOn(frame,Components(fPDFs[ic]),LineStyle(kDashed),LineColor(ic%8+1),Precision(1E-2)) ; //just the back fit result  
    frame->Draw() ;
    canvas->Modified();
    canvas->Update();
  }
}

THSRooFit*  THSRooFit::CreateSubFit(TNamed cut){//cut.fName=cut selectionl fcut.fTitle=name
  cout<<"CreateSubFit "<<cut.GetName()<<endl;
  //create a fit object for a subset of data either by setting cut
  //or by fTree->SetEntryList prior to calling this function 
  //It will be deleted by this object
  THSRooFit* RFa=new THSRooFit();
  fRooFits->Add(RFa);
  RFa->SetName(cut.GetName());
  if(fBinnedFit)RFa->SetBinnedFit();
  RFa->SetSingleSpecies(fSingleSp);
  RFa->SetOutDir(fOutDir);
  // RFa->SetSPlotRange(fSRange[0],fSRange[1]);
  RFa->SetInWeights(fInWeights);
  RFa->SetWeightName(fWeightName);
  RFa->LoadWorkSpace(GetWorkSpace());
  //speed up copy by turning off redundant branches
  // RFa->SetBranchStatus("*",0);
  //need iterator over fVariables and getName 
  RFa->LoadDataSet(GetTree()->CopyTree(cut.GetTitle()));//will use any EntryList
  RFa->SetDataWeight();//if defined weights use them for this dataset
  return RFa;
}
THSRooFit*  THSRooFit::CreateSubFitBins(TNamed cut){//cut.fName=cut selectionl fcut.fTitle=name
  cout<<"THSRooFit::CreateSubFitBins "<<cut.GetName()<<endl;
  //create a fit object for a subset of data either by setting cut
  //or by fTree->SetEntryList prior to calling this function 
  //It will be deleted by this object
  THSRooFit* RFa=new THSRooFit();
  fRooFits->Add(RFa);
  if(fBinnedFit)RFa->SetBinnedFit();
  RFa->SetSingleSpecies(fSingleSp);
  RFa->SetOutDir(fOutDir);
  // RFa->SetSPlotRange(fSRange[0],fSRange[1]);
  RFa->SetInWeights(fInWeights);
  RFa->SetWeightName(fWeightName);
  RFa->SetName(cut.GetName());
   RFa->LoadWorkSpace(fWS);
   //speed up copy by turning off redundant branches
  fTree->SetBranchStatus("*",0);
  //fTree->SetCacheSize(30000000);
  for(Int_t i=0;i<fVariables.getSize();i++){//only copy variable branches for speed
    fTree->SetBranchStatus(fVariables[i].GetName(),1);
    //fTree->AddBranchToCache(fVariables[i].GetName());//??testing if this is faster
  }
  //but always need ID branch
  if(fTree->GetBranch(fIDBranchName)){
   fTree->SetBranchStatus(fIDBranchName,1);
  }
  else cout<<"Warning : THSRooFit::CreateSubFitBins no ID branch set, omitting, and will not be able to save weights"<<endl;
  //need iterator over fVariables and getName 
  RFa->LoadDataSet(fTree->CopyTree(cut.GetTitle()));//will use any EntryList
  fTree->SetBranchStatus("*",1);
  RFa->SetDataWeight();//if defined weights use them for this dataset
  return RFa;
}
THSRooFit*  THSRooFit::CreateSubFitBins(TTree* ctree,Bool_t CopyTree){//events already selected
  cout<<"THSRooFit::CreateSubFitBins with tree "<<ctree->GetName()<<endl;
  //create a fit object for a subset of data either by setting cut
  //or by fTree->SetEntryList prior to calling this function 
  //It will be deleted by this object
  cout<<fOutDir<<endl;
  THSRooFit* RFa=new THSRooFit();
  cout<<"1"<<" "<<RFa<<endl;
  cout<<fIDBranchName<<endl;
  RFa->SetIDBranchName(fIDBranchName);
  cout<<"1b"<<endl;
  RFa->SetName(ctree->GetName());
  cout<<"2"<<endl;
  if(fBinnedFit)RFa->SetBinnedFit();
  RFa->SetSingleSpecies(fSingleSp);
  RFa->SetBinDir(fBinDir);
  RFa->SetOutDir(fOutDir);
  RFa->SetInWeights(fInWeights);
  RFa->SetWeightName(fWeightName);
  RFa->SetNStudyTrials(fNStudyTrials);
  RFa->SetStudyPDF(fStudyPDF);
  
  //Done configuring RF
  fRooFits->Add(RFa);
  cout<<"3"<<endl;
  RFa->LoadWorkSpace(fWS);
  cout<<"4"<<endl;
 
  for(Int_t ill=0;ill<fFitOptions.GetSize();ill++)
    RFa->AddFitOption(*((RooCmdArg*)fFitOptions.At(ill)));
  TDirectory *saveDir=gDirectory;
  if(ctree->GetDirectory())ctree->GetDirectory()->cd();
  if(CopyTree)RFa->LoadDataSet(ctree->CopyTree(""));//will use any EntryList
  else RFa->LoadDataSet(ctree);//use whole tree
  saveDir->cd();
  RFa->SetDataWeight();//if defined weights use them for this dataset
  return RFa;
}
void THSRooFit::SavePlots(TString filename){
  TFile* file=0;
  cout<<"SAVING PLOTS "<<TString(fOutDir+TString("Results")+GetName()+".root")<<" "<<fCanvases<<" "<<fResult<<endl;
			  
  file=new TFile(fOutDir+TString("Results")+GetName()+".root","recreate");
  if(fCanvases) fCanvases->Write();
  if(fResult) fResult->Write("HSFitResult");
  file->Close();
  delete file;
}
void THSRooFit::MakeBins(){
  //also include fit variables in bins as THSBins can then be used to check events
  // for(Int_t ib=0;ib<fVariables.getSize();ib++)//only if RooRealVar, not for example RooCategory
    // if(dynamic_cast<RooRealVar*>(&fVariables[ib]))fDataBins->AddAxis(fVariables[ib].GetName(),1,((RooRealVar*)&fVariables[ib])->getMin(""),((RooRealVar*)&fVariables[ib])->getMax(""));
  fDataBins->InitialiseBinTree("HSDataBins",fOutDir+"DataEntries.root");
  fDataBins->RunBinTree(fTree);
  fDataBins->Save();

  if(fMCIntTree){//If given a tree for NormInt calc mke bins of it here
    fDataBins->InitialiseBinTree("HSMCIntBins",fOutDir+"MCIntEntries.root");
    fDataBins->RunBinTree(fMCIntTree);
    fDataBins->Save();
  }
  if(fMCGenTree){//If given a tree for NormInt Gen calc mke bins of it here
    fDataBins->InitialiseBinTree("HSMCGenBins",fOutDir+"MCGenEntries.root");
    fDataBins->RunBinTree(fMCGenTree);
    fDataBins->Save();
  }
}
void THSRooFit::MakeBins(TTree* tree,TString name){
  if(!fDataBins) {Warning("THSRooFit::MakeBins","Trying to make bins but no bins defined");return;}
  fDataBins->InitialiseBinTree("HSBins",fOutDir+name+"Entries.root");
  fDataBins->RunBinTree(tree);
  fDataBins->Save();
}
void THSRooFit::MakeBinnedTrees(TTree* tree,TString name){
  MakeBins(tree,name);

  tree->SetBranchStatus("*",0);
  for(Int_t i=0;i<fVariables.getSize();i++){//only copy variable branches for speed
    tree->SetBranchStatus(fVariables[i].GetName(),1);
  }
  for(Int_t i=0;i<fAuxVars.getSize();i++){//only copy variable branches for speed
    tree->SetBranchStatus(fAuxVars[i].GetName(),1);
  }
  //but always need ID branch
  if(tree->GetBranch(fIDBranchName)){
    tree->SetBranchStatus(fIDBranchName,1);
  }

  THSBins* savedBins=new THSBins("HSDataBins",fOutDir+name+"Entries.root");
  for(Int_t i=0;i<savedBins->GetN();i++){
    gSystem->MakeDirectory(fOutDir+savedBins->GetBinName(i));
    TFile* ofile=new TFile(fOutDir+savedBins->GetBinName(i)+TString("/Tree")+name+".root","recreate");
    TTree* newtree=savedBins->GetBinnedTree(tree,i);
    newtree->SetName("BinnedTree");
    newtree->Write();
    ofile->Close();
    delete ofile;
  }
  delete savedBins;
 
}
void THSRooFit::ConfigureSavedBins(TString dirname){
  fBinDir=dirname;
  fDataBins=new THSBins("HSDataBins",fBinDir+"DataEntries.root");
  if(!fDataBins->GetN()) {Error("THSRooFit::ConfigureSavedBins()","No bins found in directory = %s",fBinDir.Data());return;}
  return;
}
void THSRooFit::FitWithBins(Int_t Nfits){
  if(!fWS->set("PDFs"))DefineSets();
  MakeBins();
  cout<<"THSRooFit::FitWithBins(); number of bins "<<fDataBins->GetN()<<endl;
  TDirectory *saveDir=gDirectory;
  THSBins* savedBins=new THSBins("HSDataBins",fOutDir+"DataEntries.root");
  fTree->SetBranchStatus("*",0);
  for(Int_t i=0;i<fVariables.getSize();i++){//only copy variable branches for speed
    fTree->SetBranchStatus(fVariables[i].GetName(),1);
  }
  //but always need ID branch
  if(fTree->GetBranch(fIDBranchName)){
   fTree->SetBranchStatus(fIDBranchName,1);
  }
  for(Int_t i=0;i<fDataBins->GetN();i++){
    THSRooFit* rf=CreateSubFitBins(savedBins->GetBinnedTree(fTree,i),kFALSE);
  
    if(fModel)rf->TotalPDF();//total PDF defined in parent so also for child
    rf->FitMany(Nfits);
    rf->SavePlots(fOutDir+TString("Results")+fDataBins->GetBinName(i)+".root");

    delete rf;
  }
  cout<<"THSRooFit::FitWithBins() Done all Fits "<<endl;
  ///??delete savedBins;
}
void THSRooFit::FitSavedBins(Int_t Nfits){
  if(!fDataBins->GetN()) return;
  Info("THSRooFit::FitSaved","Goint to run %d fits from %s",Nfits,fBinDir.Data());
  if(!fWS->set("PDFs"))DefineSets();
  TDirectory *saveDir=gDirectory;
  //Loop over bins
  for(Int_t ib=0;ib<GetBins()->GetN();ib++){
    // Fit1SavedBin(ib,Nfits);
    TChain *chainData=new TChain("BinnedTree");
    chainData->Add(GetBinDir()+GetBins()->GetBinName(ib)+TString("/Tree")+"Data"+".root");
    cout<<"Data chain "<<GetBinDir()+GetBins()->GetBinName(ib)+TString("/Tree")+"Data"+".root"<<" "<<chainData->GetEntries()<<" "<<chainData->GetName()<<endl;
    THSRooFit* rf=CreateSubFitBins(chainData,kFALSE);
    rf->SetName(GetBins()->GetBinName(ib));
    //look for RooHSEventsPDFs to get MC events trees
    for(Int_t ip=0;ip<fPDFs.getSize();ip++){
      RooAbsPdf* pdf=rf->GetWorkSpace()->pdf(fPDFs[ip].GetName());
      RooHSAbsEventsPDF* hspdf=0;
      if((hspdf=dynamic_cast<RooHSAbsEventsPDF*>(pdf))){    
	Info("HSRooFit::FitSaved","Found RooHsAbsEventsPDF %s",hspdf->GetName());
	cout<<"MC CHAIN "<<fPDFs[ip].GetName()<<endl;
	TChain *chainMC=new TChain("BinnedTree");
	//pdf has ownership of chain when set
	chainMC->Add(GetBinDir()+GetBins()->GetBinName(ib)+TString("/Tree")+hspdf->GetName()+".root");
	if(!hspdf->SetEvTree(chainMC)) {Error("THSRooFit::FitSavedBins","problem with chain for %s",hspdf->GetName());exit(0);}
      }
    } 
    //Configured the fir for this bin now do it
    if(fModel)rf->TotalPDF();//total PDF defined in parent so also for child
    rf->FitAndStudy(Nfits);
    delete rf;
    delete chainData;
  }
  
}
void THSRooFit::FitBatchBin(Int_t Nfits){
  
  Info("THSRooFit::FitBatchBin","Goint to run fit from %s",fBinDir.Data());
  if(!fWS->set("PDFs"))DefineSets();
  TDirectory *saveDir=gDirectory;

  TChain *chainData=new TChain("BinnedTree");
  chainData->Add(GetBinDir()+TString("/Tree")+"Data"+".root");
  cout<<"Data chain "<<GetBinDir()+TString("/Tree")+"Data"+".root"<<" "<<chainData->GetEntries()<<" "<<chainData->GetName()<<endl;
  LoadDataSet(chainData);
 //THSRooFit* rf=CreateSubFitBins(chainData,kFALSE);
  //fBinDir.ReplaceAll("/","");
  // rf->SetName(fBinDir);
  //look for RooHSEventsPDFs to get MC events trees
  for(Int_t ip=0;ip<fPDFs.getSize();ip++){
    RooAbsPdf* pdf=fWS->pdf(fPDFs[ip].GetName());
    RooHSAbsEventsPDF* hspdf=0;
    if((hspdf=dynamic_cast<RooHSAbsEventsPDF*>(pdf))){    
      Info("HSRooFit::FitSaved","Found RooHsAbsEventsPDF %s",hspdf->GetName());
      cout<<"MC CHAIN "<<fPDFs[ip].GetName()<<endl;
      TChain *chainMC=new TChain("BinnedTree");
      //pdf has ownership of chain when set
      chainMC->Add(GetBinDir()+TString("/Tree")+hspdf->GetName()+".root");
      if(!hspdf->SetEvTree(chainMC)) {Error("THSRooFit::FitSavedBins","problem with chain for %s",hspdf->GetName());exit(0);}
    }
  } 
  //Configured the fir for this bin now do it
  //if(fModel) TotalPDF();//total PDF defined in parent so also for child
  FitAndStudy(Nfits);
  // delete rf;
  delete chainData;
  
}
void THSRooFit::FitAndStudy(Int_t Nfits){
   //Create new fit and load the new bin data tree
  if(!fWS->set("PDFs"))DefineSets();
  FitMany(Nfits);
  Info("THSRooFit::Fit1SavedBin","Save to %s",(fOutDir+TString("Results")+GetName()).Data());
  SavePlots(fOutDir+TString("Results")+GetName()+".root");
  StudyFit();
  
}
void THSRooFit::StudyFit(){
  if(!fNStudyTrials) return;
  if(!fWS->set("PDFs"))DefineSets();

  Info("THSRooFit::StudyFit()","will attempt %d trials on %s",fNStudyTrials,fStudyPDF.Data());
  RooHS1StepStudy this_study(GetName(),"testing the fit systematics");
  this_study.attach(*fWS);
  this_study.SetOutDir(GetOutDir());
  this_study.SetPDFName(fStudyPDF);
  this_study.SetPlot(fStudyPlot);
  
  if(fBinnedFit)this_study.GetRooFit()->SetBinnedFit();
  for(Int_t ill=0;ill<fFitOptions.GetSize();ill++)
    this_study.GetRooFit()->AddFitOption(*((RooCmdArg*)fFitOptions.At(ill)));
  RooStudyManager mgr(*fWS,this_study);
  mgr.run(fNStudyTrials) ;
}
void THSRooFit::FitMany(Int_t Nfits){
  //Do the fit many times with different initial paramters
  //Maybe new fit so construct PDF if not laready loaded
  if(!fWS->set("PDFs"))DefineSets();
    // if(!fModel)TotalPDF();
 
  //Store the likelihood value
  TVectorD loglh(Nfits);
  //Fit the model to data with all paramters free
  //first fit use initial paramter values
  if(!fModel) fModel=(RooAbsPdf*)&(fPDFs[0]);//Not ideal, will just take the fist PDF loaded by LoadSpecies unless TotalPDF has already been called.
  Fit();
  //plot result
  PlotDataModel();
  if(fBinnedFit&&!TMath::IsNaN(fChi2)) loglh[0]=fChi2; //actually did chi2 fit
  else if(!(TMath::IsNaN(fResult->minNll())||fResult->minNll()<-0.999e+30))  loglh[0]=fResult->minNll();
  else loglh[0]=1E300;
  //((TCanvas*)fCanvases->Last())->SetTitle(Form("Fit %d LogL = %lf",0,fChi2));
  fWS->saveSnapshot(Form("ssFit%d",0),RooArgSet(fYields,fParameters),kTRUE);
  TObjArray* fitResults=new TObjArray(Nfits);
  fitResults->SetOwner(kTRUE);
  fitResults->AddLast((RooFitResult*)fResult->clone());
  for(Int_t ifit=1;ifit<Nfits;ifit++){
    Fit(kTRUE);
    //   fWS->saveSnapshot(Form("ssFit%d",ifit),*(fModel->getVariables()),kTRUE);
    fWS->saveSnapshot(Form("ssFit%d",ifit),RooArgSet(fYields,fParameters),kTRUE);
    //fWS->saveSnapshot(Form("YssFit%d",ifit),fYields,kTRUE);
  //plot result
    PlotDataModel();
    // ((TCanvas*)fCanvases->Last())->SetTitle(Form("Fit %d LogL = %lf",ifit,fChi2));
    fitResults->AddLast((RooFitResult*)fResult->clone());
    //Can only get chi2 after PlotDataModel
    if(fBinnedFit&&!TMath::IsNaN(fChi2)) loglh[ifit]=fChi2; //actually did chi2 fit
    else if((TMath::IsNaN(fResult->minNll())||fResult->minNll()!=-1e+30))loglh[ifit]=fResult->minNll();  //loglh[ifit]=fResult->minNll();
    else loglh[ifit]=1E300;
    // cout<<"MANY FITS "<<fResult->covQual()<<endl;
    // if(fResult->status()==0)loglh[ifit]=fResult->minNll();
    //else   loglh[ifit]=1E300;

  }
  cout<<"THSRooFit::FitMany  Likelihoods "<<endl;
  for(Int_t i=0;i<Nfits;i++){
    cout<<loglh[i]<<endl;
  }	
  //take best result for splot parameters
  Int_t best=TMath::LocMin(Nfits,loglh.GetMatrixArray());
  cout<<"THSRooFit::FitMany() Best likelihood was "<<loglh[best]<<" "<<best<<" so use those parameters"<<endl;
  fWS->loadSnapshot(Form("ssFit%d",best));
 
  fResult=dynamic_cast<RooFitResult*>(fitResults->RemoveAt(best));//keep for writing to file
  fResult->SetName(Form("Result"));
  fResult->Print();

  for(Int_t iy=0;iy<fYields.getSize();iy++){
    cout<<((RooRealVar*)&fYields[iy])->GetName()<<" "<<((RooRealVar*)&fYields[iy])->getVal()<<endl;
    //  ((RooRealVar*)&fYields[iy])->setError(sqrt(((RooRealVar*)&fYields[iy])->getVal()));
  }

}

void THSRooFit::PrepareForFarm(){
  if(!fWS->set("PDFs"))DefineSets();
  MakeBins();
  cout<<"THSRooFit::PrepareForFarm(); number of bins "<<fDataBins->GetN()<<endl;
  TDirectory *saveDir=gDirectory;
  //Prepare data to fit
  THSBins* savedBins=new THSBins("HSDataBins",fOutDir+"DataEntries.root");
  fTree->SetBranchStatus("*",0);
  for(Int_t i=0;i<fVariables.getSize();i++){//only copy variable branches for speed
    fTree->SetBranchStatus(fVariables[i].GetName(),1);
  }
  //but always need ID branch
  if(fTree->GetBranch(fIDBranchName)){
   fTree->SetBranchStatus(fIDBranchName,1);
  }

  //Prepare MCIntTree if exists in the same way
  THSBins* mcintBins=0;
  if(fMCIntTree){
    mcintBins=new THSBins("HSMCIntBins",fOutDir+"MCIntEntries.root");
    fMCIntTree->SetBranchStatus("*",0);
    for(Int_t i=0;i<fVariables.getSize();i++){//only copy variable branches for speed
      fMCIntTree->SetBranchStatus(fVariables[i].GetName(),1);
    }
  }
  //Prepare MCGenTree if exists in the same way
  THSBins* mcgenBins=0;
  if(fMCGenTree){
    mcgenBins=new THSBins("HSMCGenBins",fOutDir+"MCGenEntries.root");
    fMCGenTree->SetBranchStatus("*",0);
    for(Int_t i=0;i<fVariables.getSize();i++){//only copy variable branches for speed
      fMCGenTree->SetBranchStatus(fVariables[i].GetName(),1);
    }
  }
  //Now loop over bins and create sub fits
  for(Int_t i=0;i<fDataBins->GetN();i++){	
    THSRooFit* rf=CreateSubFitBins(savedBins->GetBinnedTree(fTree,i),kFALSE);
    //Save workspace to file. This will fitted in a seperate job
    rf->GetWorkSpace()->writeToFile(fOutDir+TString("Farm")+fDataBins->GetBinName(i)+".root");
    // if(fMCIntTree)rf->LoadMCIntTree(mcintBins->GetBinnedTree(fMCIntTree,i));
    if(fMCIntTree){//Also save mcint tree to file
      TFile* ofile=new TFile(fOutDir+TString("Farm")+fDataBins->GetBinName(i)+".root","update");
      TTree* mctree=mcintBins->GetBinnedTree(fMCIntTree,i);
      mctree->SetName("HSMCIntTree");
      mctree->SetDirectory(ofile);
      mctree->Write();
      ofile->Close();
      delete ofile;
    }
    if(fMCGenTree){//Also save mcgen tree to file
      TFile* ofile=new TFile(fOutDir+TString("Farm")+fDataBins->GetBinName(i)+".root","update");
      TTree* mctree=mcgenBins->GetBinnedTree(fMCGenTree,i);
      mctree->SetName("HSMCGenTree");
      mctree->SetDirectory(ofile);
      mctree->Write();
      ofile->Close();
      delete ofile;
    }

    cout <<"void THSRooFit::PrepareForFarm() Saved Workspace with "<<rf->GetDataSet()->numEntries()<<" for " <<fDataBins->GetBinName(i)<<endl;

    rf->RemoveDataSet();//save memory

    delete rf;
  }
  delete savedBins;
  delete mcintBins;
  delete mcgenBins;
  
  cout<<"THSRooFit::PrepareForFarm() Done all files "<<endl;
}
void THSRooFit::WriteToFile(TString fname){
  //this currently crashes and could be fixed
  //need to change the version to 1 in ClassDef
  TFile rfFile(fname,"recreate");
  Write();
  rfFile.Close();
}
Bool_t THSRooFit::InitialiseFit(){
  cout<<" THSRooFit::InitialiseFit()"<<endl;
  if(GetDataSet()->numEntries()<2) {return kFALSE;}
  if(GetPDFs().getSize()==0){
    cout<<" THSRooFit::InitialiseFit() no model found for "<<GetName()<<" probably no events kinmatically allowed in this bin" <<endl;
    return kFALSE;
  }
  return kTRUE;
    
}
void THSRooFit::DefaultFitOptions(){
  // AddFitOption(RooFit::Extended());
  cout<<fFitOptions.GetSize()<<" "<<fData<<endl;
  if(fData)
    if(fData->isNonPoissonWeighted())AddFitOption(RooFit::SumW2Error(kTRUE));
  AddFitOption(RooFit::NumCPU(1));
  AddFitOption(RooFit::Save(kTRUE));
  AddFitOption(RooFit::Warnings(kFALSE));

  //if(!fFitOptions.FindObject(&extend)) fFitOptions.Add(&extend);
  //if(!fFitOptions.FindObject(&sumw2))fFitOptions.Add(&sumw2);
  //if(!fFitOptions.FindObject(&ncpu))fFitOptions.Add(&ncpu);
  //if(!fFitOptions.FindObject(save))fFitOptions.Add(save);
  //if(!fFitOptions.FindObject(&warn))fFitOptions.Add(&warn);

}
