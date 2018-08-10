//Run with 
//root --hsfit FitHSAsymmetry.C --PhiAsymmetry.cxx

{
  THSRooFit* RF=new THSRooFit("AFit");
  RF->SetOutDir("outAsymmetryMCInt/");
 ///////////////////////////////Load Variables
  RF->LoadVariable("Phi[-180,180]");//going to fit this

  RF->LoadBinVars("Mmiss",1,0,10);//for consistency same range as signal fit
  RF->LoadBinVars("Eg",4,3,4);//here use smae energy bins

  ///////////////////////////Load MC
  TChain chainMC("MyModel");
  chainMC.Add("MC.root");
  RF->SetIDBranchName("fgID");
  //////////////////////////////Split data into bins and load them
  RF->MakeBinnedTrees(&chainMC,"SigAsym"); //Note SigAsym-->PhiAsymmetry::SigAsym 
  ///////////////////////////Load Data
  //Add data specific branches
  RF->LoadVariable("Pol[0,1]");//but fix polarisation event-by-event
  RF->LoadVariable("PolState[Polp=1,Polm=-1]");//important this syntax loads the integer PolState as a RooCategory which is required for asymmetries, not RooRealVar
  TChain chain("MyModel");
  chain.Add("Data.root");
  RF->SetIDBranchName("fgID");
  //////////////////////////////Split data into bins and load them
  RF->MakeBinnedTrees(&chain,"Data");


  
  RF->ConfigureSavedBins(RF->GetOutDir());
  //Add integrated weights file
  RF->LoadWeights("outWeights/binFitTweights.root ","HSsWeights");
  RF->SetWeightName("Signal"); //Same as Signal species in FitHSSimpleBins

  /////////////////////////////Make asymmetry model A and B are fit parameters
  RF->Factory("PhiAsymmetry::SigAsym( Phi,Pol,PolState,A[0,-1,1],B[0,-1,1] )");
  RF->LoadSpeciesPDF("SigAsym");
 

  gBenchmark->Start("Binned");
  RF->FitSavedBins(1,kFALSE);//FALSE so can use fits to draw asymmetries
  gBenchmark->Stop("Binned");
  gBenchmark->Print("Binned");


  //example of how to draw asymmetries
  RooCategory *polState=RF->GetWorkSpace()->cat("PolState");
  for(Int_t ib=0;ib<RF->GetFits()->GetEntries();ib++){
    RooAbsPdf* bpdf=((THSRooFit*)RF->GetFits()->At(ib))->GetModel();
    RooAbsData* bdata=((THSRooFit*)RF->GetFits()->At(ib))->GetData();
    RooRealVar* vpol=((THSRooFit*)RF->GetFits()->At(ib))->GetWorkSpace()->var("Pol");
    //Pol is a variable so it will average flat pol over whole range
    //Here we are better with the average polarisation from the data in this bin
    //This can be done by projecting a slice in Pol around the mean value...
    TString RName=Form("Rpol%d",ib);
    vpol->setRange(RName.Data(),bdata->mean(*vpol)-0.01,bdata->mean(*vpol)+0.01);

    RooPlot* frameX= RF->GetWorkSpace()->var("Phi")->frame();
    bdata->plotOn(frameX,RooFit::Asymmetry(*polState),RooFit::MarkerColor(kBlue)); 
    bpdf->plotOn(frameX,RooFit::ProjectionRange(RName.Data()),RooFit::Asymmetry(*polState),RooFit::LineColor(kBlue))  ;
    new TCanvas();
    frameX->Draw();
  }


}
