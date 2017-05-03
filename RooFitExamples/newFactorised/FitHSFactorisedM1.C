//Run with 
//root --hsfit FitHSFactorisedMmiss.C

{
  THSsPlot* RF=new THSsPlot("SF");
  RF->SetOutDir("outM1/");
  ///////////////////////////////Load Variables
  RF->LoadVariable("M1[0,10]");//should be same name as variable in tree 
  RF->LoadBinVars("Mmiss",1,0,10);//Not to be fitted but limits applied to dataset
  RF->LoadAuxVars("Mmiss[0,0,10]");//Not to be fitted but limits applied to dataset
  RF->LoadBinVars("Eg",10,3,4);//Not to be fitted but limits applied to dataset
  RooRealVar * var=RF->GetWorkSpace()->var("M1");
  var->setBins(100); //number of bins used in PDF histogram
  RF->SetIDBranchName("fgID");

   //Split data into bins
  //MC signal
  TChain *chainmc=new TChain("MyModel","mcsignal");
  chainmc->Add("SigDataMulti.root");
  RF->MakeBinnedTrees(chainmc,"Sig1");
  //MC background
  TChain *chainmcb=new TChain("MyModel","mcsignal");
  chainmcb->Add("BG1DataMulti.root");
  RF->MakeBinnedTrees(chainmcb,"BG1");
  //data to be fitted
  TChain chain("MyModel");
  chain.Add("DataMulti.root");
  RF->MakeBinnedTrees(&chain,"Data");
  RF->ConfigureSavedBins("outM1/"); 

  //////////////////////////////Make signal PDF
  RF->Factory("RooHSEventsHistPDF::Sig1(M1,alpha[0,0,20],off[0,-0,0],scale[1,1,1])");
  RooHSEventsHistPDF* pdf=dynamic_cast<RooHSEventsHistPDF*>(RF->GetWorkSpace()->pdf("Sig1"));
  //Attach MC signal data
  // TChain *chainmc=new TChain("MyModel","mcsignal");
  // chainmc->Add("SigDataMulti.root");
  // pdf->SetEvTree(chainmc);
  RF->LoadSpeciesPDF("Sig1",1); 
  //////////////////////////////Make background PDF
  RF->Factory("RooHSEventsHistPDF::BG1(M1,alphaB[0,0,20],offB[0,0,0],scaleB[1.0,1,1])");
  RooHSEventsHistPDF* pdfb=dynamic_cast<RooHSEventsHistPDF*>(RF->GetWorkSpace()->pdf("BG1"));
  // //Attach MC background data
  // TChain *chainmcb=new TChain("MyModel","mcbackground");
  // chainmcb->Add("BGDataMulti.root");
  // pdfb->SetEvTree(chainmcb);
  RF->LoadSpeciesPDF("BG1",1);
  //Add data to chain
  // TChain chain("MyModel");
  // chain.Add("DataMulti.root");
  //import to RooFit
  // RF->LoadDataSet(&chain);
  // //Load previous weights
  RF->LoadWeights("outMmiss/WeightsSF.root","HSsWeights");
  RF->SetWeightName("Sig");

  gBenchmark->Start("M1Fit");
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
  gBenchmark->Stop("M1Fit");
  gBenchmark->Print("M1Fit");
  THSWeights* wts=new THSWeights("TotalWeights");
  wts->Merge("outM1/WeightsEg","outM1/WeightsTotal.root","HSsWeights");
  RF->SetTree(&chain);
  RF->SetWeights(wts);
  // RF->DrawTreeVar("Mmiss",100,0,10);
  // RF->DrawTreeVar("M1",100,0,10);
  // RF->DrawTreeVar("M2",100,0,10);


}
