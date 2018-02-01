void Analyse(RooDataSet *toyResults,THSRooFit* aRF,TString fitname);

void MergeResults(Int_t Ntoys,TString fitname){
  
  TString filedir=TString(gSystem->Getenv("FILEDIR"));
  TString outdir=TString(gSystem->Getenv("OUTDIR"));

  // TString create=TString(gSystem->Getenv("CREATEMODEL"));
  // gROOT->LoadMacro(create);
  THSRooFit *aRF=RF; //needs edited by hand!!
 
  RooDataSet* totalData=nullptr;
  
  //Merge all the fit result data sets

  for(Int_t i=0;i<Ntoys;i++){
    TFile* file=new TFile(filedir+"/"+outdir+"/"+"Results"+fitname+Form("_Range_%d",i)+".root");
    RooDataSet* data=(RooDataSet*)file->Get("HSResults");
    if(!totalData){
      const RooArgSet* fitVars=data->get(0);
      totalData=new RooDataSet("intResults","intResults",*fitVars,RooFit::StoreError(*fitVars));
    }
    const RooArgSet* fitVars=data->get(0);
    totalData->add(*fitVars);
    delete data;
    file->Close();
    delete file;
  }
  totalData->Print("v");
  Analyse(totalData,aRF,fitname);
  
}

void Analyse(RooDataSet *toyResults,THSRooFit* aRF,TString fitname){
  
  RooWorkspace* WS=aRF->GetWorkSpace();	
 TString outdir=TString(gSystem->Getenv("OUTDIR"));
 TString filedir=TString(gSystem->Getenv("FILEDIR"));
 
 //Get full fit results
 TFile* fileA=new TFile(filedir+"/"+outdir+"/"+"Results"+fitname+".root");
 RooDataSet* dataA=(RooDataSet*)fileA->Get("HSResults");
 const RooArgSet* AVars=dataA->get();

 TFile outfile(filedir+"/"+outdir+"/HSRanges"+aRF->GetName()+".root","recreate");

  toyResults->Print();
  const RooArgSet* fitVars=toyResults->get();
  fitVars->Print("v");
  for(Int_t iv=0;iv<fitVars->getSize();iv++){
    if(iv>=aRF->GetParameters().getSize())break;
    cout<<iv<<" "<<aRF->GetParameters().getSize()<<endl;
    RooRealVar* var=(RooRealVar*)(fitVars->find(aRF->GetParameters()[iv].GetName()));
    
    if(!var) continue;
    var->setRange(-0.5,0.5);
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
    // RooRealVar* genPar =(RooRealVar*) ((RooRealVar*)WS->var(var->GetName()))->Clone(TString(var->GetName())+"truth");
    RooRealVar* genPar =(RooRealVar*) ((RooRealVar*)AVars->find(var->GetName()))->Clone(TString(var->GetName())+"truth");
    //RooRealVar genPar(*genPar0);
    //RooRealVar genPar("gen","gen",0.5);
    genPar->Print("");
    var->Print();
    RooPullVar pull(TString(var->GetName())+"Pull",TString(var->GetName())+" Pull",*var,*genPar);
    pull.Print("");
    toyResults->addColumn(pull,kFALSE) ;
    WS->import(pull);
    
    RooRealVar pvar(TString(var->GetName())+"Pull",TString(var->GetName())+" Pull",-5,5) ;
    pvar.Print("");
    
    RooPlot *framePull=pvar.frame();
    toyResults->plotOn(framePull);
    
    RooRealVar mp("mp","mp",0,-5,5) ;
    RooRealVar sp("sp","sp",1,0,10);
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
