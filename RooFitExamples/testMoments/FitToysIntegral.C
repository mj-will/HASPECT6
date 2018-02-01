///////////////////////////////////////////////////
///run with
/// set the file name to the GenerateToys directory e.g.
/// setenv FILEDIR $PWD/Toys_A
/// Set an extra directory to put output into e.g.
/// setenv OUTDIR RunIntegral
/// The fitting script e.g.
/// setenv RUNMAC $PWD/FitToysIntegral.C
/// setenv CREATEMODEL $PWD/CreateModel.C
/// The first argument=number toy fits, last arguemnt =1 ->qsub 0 =>run local
/// root 'HSSubmitToys.C(1,1)'
{

  gROOT->LoadMacro("CreateModel.C");
  RF->SetOutDir("Toys_A/RunIntegral100");
  //  RF->SetOutDir("./");
  
  //Get data for fitting
  TDirectory* savedir=gDirectory;
  TFile* datafile=new TFile("ToyData.root");
  RooDataSet* ds = dynamic_cast<RooDataSet*>( datafile->Get("ToyData"));
  savedir->cd();
  gDirectory=savedir;
  
  RF->LoadDataSet(ds);
  
  
  //Try 5 fits with different initial parameters
  RF->SetFitMethod(1); //use standard roofit
  RF->FitAndStudy(1);
  RF->SavePlots("");

  RF->AddHSPDF_Ranges("YLM");
  RF->SetNIntRanges(100);
  // ((RooHSEventsPDF*)RF->GetWorkSpace()->pdf("YLM"))->SetIntRange(1,444);
  RF->FitSplitIntegral(1);

  datafile->Close();
  delete datafile;
  delete ds;
  delete RF;
}
