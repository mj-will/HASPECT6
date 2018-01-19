{

 //Create THSRooFit manager 
  THSRooFit* RF=new THSRooFit("AFit");
  TString filedir=gSystem->Getenv("PWD");
  RF->SetOutDir(filedir); //give output directory

  //Load the variables for fitting, these should be branches in you tree
  //see MakeEventsRes.C
  RF->LoadVariable("Z[-1,1]");//theta
  RF->LoadVariable("Phi[0,-3.14159,3.14159]");//phi
  //Set the max L and M for the moments
  Int_t LMAX=4;
  Int_t MMAX=2;
  //Create RooRealVars for all moments
  RooArgSet pars=RooHSSphHarMoments::CreateMoments(LMAX,MMAX);
  RF->GetWorkSpace()->import(pars);
  RF->GetWorkSpace()->defineSet("Moments",pars);

  //Construct Moments PDF for generating/Fitting with
  RooHSSphHarMoments* pdf=new RooHSSphHarMoments("YLM","YLM",*(RF->GetWorkSpace()->var("Z")) ,*(RF->GetWorkSpace()->var("Phi")),LMAX,MMAX,*RF->GetWorkSpace()->set("Moments"));
  //Add data (see MakeEventsRes.C
  TChain *chain=new TChain("decayAngles");
  // chain.Add("accepted_res.root");
  // chain->Add("phasespace_res.root");
  pdf->SetEvTree(chain,RF->GetCut());
  pdf->SetNInt(100000);//Number of events to use in integration calc.
  pdf->SetUseWeightsGen(kFALSE); //Use accept/reject not weights
  RF->GetWorkSpace()->import(*pdf); //import pdf into workspace

  //Load PDF into RooFit Manager and set to generate 1000 events
  RF->LoadSpeciesPDF("YLM",10000);
  RF->TotalPDF(); //Make extended PDF
  //Choose generated moments
  //Warning some combinations of moments can give negative PDFs
  //It would be good if we could check for this before running?
  RF->GetWorkSpace()->var("MomY_1_1")->setVal(0.3);
  RF->GetWorkSpace()->var("MomY_2_0")->setVal(-0.2);
  RF->GetWorkSpace()->var("MomY_4_2")->setVal(0.1);
  RF->GetWorkSpace()->var("MomY_4_1")->setVal(-0.2);


}
