void Analyse(RooDataSet *toyResults,THSRooFit* aRF);

void MergeResults(Int_t Ntoys){
  
  //Load fit model
  gROOT->LoadMacro(gSystem->Getenv("HS_FITMODEL"));
  THSRooFit *fitRF=HS::gRF;
  TString fitname= fitRF->GetName();
  //Load gen model
  gROOT->LoadMacro(gSystem->Getenv("HS_GENMODEL"));
   THSRooFit *aRF=HS::gRF; 
 
  TString filedir=TString(gSystem->Getenv("HS_FILEDIR"));
  TString outdir=TString(gSystem->Getenv("HS_OUTDIR"));
  
 
  RooDataSet* totalData=nullptr;
  
  for(Int_t i=0;i<Ntoys;i++){
    TFile* file=new TFile(filedir+"/"+outdir+"/"+Form("HSFarm_%d/",i)+"Results"+fitname+".root");
    if(!file->IsOpen()) continue;
    RooDataSet* data=(RooDataSet*)file->Get("HSResults");
    if(!totalData){
      const RooArgSet* fitVars=data->get(0);
      totalData=new RooDataSet("toyResults","toyResults",*fitVars,RooFit::StoreError(*fitVars));
    }
    const RooArgSet* fitVars=data->get(0);
    totalData->add(*fitVars);
    delete data;
    file->Close();
    delete file;
  }
  totalData->Print("v");
  Analyse(totalData,aRF);
  
  delete aRF;
  delete fitRF;
}

void Analyse(RooDataSet *toyResults,THSRooFit* aRF){
  
  RooWorkspace* WS=aRF->GetWorkSpace();
 TString outdir=TString(gSystem->Getenv("HS_OUTDIR"));
 TString filedir=TString(gSystem->Getenv("HS_FILEDIR"));
 TFile outfile(filedir+"/"+outdir+"/HSStudy"+aRF->GetName()+".root","recreate");

  toyResults->Print();
  const RooArgSet* fitVars=toyResults->get(1);
  fitVars->Print("v");
  for(Int_t iv=0;iv<fitVars->getSize();iv++){
    if(iv>=aRF->GetParameters().getSize())break;
    cout<<iv<<" "<<aRF->GetParameters().getSize()<<endl;
    RooRealVar* var=(RooRealVar*)(fitVars->find(aRF->GetParameters()[iv].GetName()));
    if(!var) continue;
   RooRealVar m("m","m",0,var->getMin(),var->getMax()) ;
    RooRealVar s("s","s",(var->getMax()-var->getMin())/10,0,(var->getMax()-var->getMin())*10);
    RooGaussian g("g","g",*var,m,s) ;
    g.fitTo(*toyResults);
    RooPlot *frame=var->frame();
    toyResults->plotOn(frame);
    g.paramOn(frame,Layout(0.1, 0.4, 0.9),
	      Format("NEU",AutoPrecision(2)),
	      ShowConstants());
    g.plotOn(frame);
    //now pulls
    RooRealVar* genPar =(RooRealVar*) ((RooRealVar*)WS->var(var->GetName()))->Clone(TString(var->GetName())+"truth");
    //RooRealVar genPar(*genPar0);
    //RooRealVar genPar("gen","gen",0.5);
    genPar->Print("");
    var->Print();
    RooPullVar pull(TString(var->GetName())+"Pull",TString(var->GetName())+" Pull",*var,*genPar);
    // pull.setRange(-100,100);
    pull.Print("");
    toyResults->addColumn(pull,kFALSE) ;
    WS->import(pull);
    
    RooRealVar pvar(TString(var->GetName())+"Pull",TString(var->GetName())+" Pull",-5,5) ;
    pvar.Print("");
    
    RooPlot *framePull=pvar.frame();
    toyResults->plotOn(framePull);
    
    RooRealVar mp("mp","mp",0.01,-10,10) ;
    RooRealVar sp("sp","sp",1.1,0.01,10);
    RooGaussian gp("gp","gp",pvar,mp,sp);
    gp.fitTo(*toyResults);
    gp.paramOn(framePull,Layout(0.1, 0.4, 0.9),
	       Format("NEU",AutoPrecision(2)),
	       ShowConstants());
    gp.plotOn(framePull);

    frame->Write();
    framePull->Write();
   }

  
  toyResults->Write();
  TTree*  treeSum=RooStats::GetAsTTree("studyTree","studyTree",*toyResults);
  treeSum->Write();
  outfile.Close();
 
}
