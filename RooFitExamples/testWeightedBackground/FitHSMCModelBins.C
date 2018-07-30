//root --hsfit  FitHSMCModelBins.C
//Now try fitting after loading the random weights into the BG model
{
  THSsPlot* RF=new THSsPlot("SF");
  RF->SetOutDir("out/");
  ///////////////////////////////Load Variables
  RF->LoadVariable("Mmiss[0.5,9.5]");//should be same name as variable in tree 
  RF->LoadBinVars("Eg",4,3,4);//should be same name as variable in tree 
  RF->SetIDBranchName("fgID");

  ///////////////////////////Load Data
  TChain chain("MyModel");
  chain.Add("Data.root"); //and background data
  RF->SetIDBranchName("fgID");
  //////////////////////////////Split data into bins and load them
  RF->MakeBinnedTrees(&chain,"Data");
  ///////////////////////////Load Data
  TChain chainSig("MyModel");
  chainSig.Add("SigData.root"); //and background data
  RF->SetIDBranchName("fgID");
  //////////////////////////////Split data into bins and load them
  RF->MakeBinnedTrees(&chainSig,"Sig");
  ///////////////////////////Load Data
  TChain chainBG("MyModel");
  chainBG.Add("BGData.root"); //and background data
  RF->SetIDBranchName("fgID");
  //////////////////////////////Split data into bins and load them
  RF->MakeBinnedTrees(&chainBG,"BG");

  RF->ConfigureSavedBins(RF->GetOutDir());
  

  
  // RF->LoadAuxVars("M2[-100,100]");//Not to be fitted but used in a cut
  RooRealVar * var=RF->GetWorkSpace()->var("Mmiss");
  var->setBins(100); //number of bins used in PDF histogram
  //  RF->SetIDBranchName("fgID");

  //////////////////////////////Make signal PDF
  RF->Factory("RooHSEventsHistPDF::Sig(Mmiss,alpha[0,0,20],off[0,-2,2],scale[1,0.8,1.2])");
  RooHSEventsHistPDF* pdf=dynamic_cast<RooHSEventsHistPDF*>(RF->GetWorkSpace()->pdf("Sig"));
  RF->LoadSpeciesPDF("Sig",1);

  
  //////////////////////////////Make background PDF
  RF->Factory("RooHSEventsHistPDF::BG(Mmiss,alphaB[0,0,5],offB[0,0,0],scaleB[1.0,0.8,1.2])");
  RF->GetWorkSpace()->var("scaleB")->setConstant();
  RF->GetWorkSpace()->var("alphaB")->setConstant();
  RF->GetWorkSpace()->var("offB")->setConstant();
  
  RooHSEventsHistPDF* pdfb=dynamic_cast<RooHSEventsHistPDF*>(RF->GetWorkSpace()->pdf("BG"));
  pdfb->LoadWeights("TimeCoinc","outBGTime/binFitTweights.root","HSsWeights");
  RF->LoadSpeciesPDF("BG",1);

  RF->LoadWeights("outDataTime/binFitTweights.root ","HSsWeights");
  RF->SetWeightName("TimeCoinc"); //Events in timing coincidence
 
  
  gBenchmark->Start("Binned");
  //Add constraints to PDF fudge parameters
  //These are derived from the alpha off and scale parameter initial val and limits
  //i.e. intital val = gaussian mean; range = 10*sigma
  RF->AddGausConstraint(pdf->AlphaConstraint());
  RF->AddGausConstraint(pdf->OffConstraint());
  RF->AddGausConstraint(pdf->ScaleConstraint());
  RF->AddGausConstraint(pdfb->AlphaConstraint());
  RF->AddGausConstraint(pdfb->OffConstraint());
  RF->AddGausConstraint(pdfb->ScaleConstraint());
  RF->FitSavedBins(1); //argument = number of test fits with random initial pars
  gBenchmark->Stop("Binned");
  gBenchmark->Print("Binned");

  TChain chain2("MyModel");
  chain2.Add("Data.root");
  RF->LoadDataSet(&chain2);
  RF->DrawTreeVar("Mmiss",100,0,10);
  RF->DrawTreeVar("M1",100,0,10);
  RF->DrawTreeVar("M2",100,0,10);

}
