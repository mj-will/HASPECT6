///////////////////////////////////////////////////
///run with
/// set the file name to the GenerateToys directory e.g.
/// setenv FILEDIR $PWD/Toys_A
/// Set an extra directory to put output into e.g.
/// setenv OUTDIR Run1
/// The fitting script e.g.
/// setenv RUNMAC $PWD/FitToys.C
/// setenv CREATEMODEL $PWD/CreateModel.C
/// The first argument=number toy fits, last arguemnt =1 ->qsub 0 =>run local
/// root 'HSSubmitToys.C(1,1)'
{

  gROOT->LoadMacro("CreateModel.C");
  RF->SetOutDir("./");
  
  //Get data for fitting
  TDirectory* savedir=gDirectory;
  TFile* datafile=new TFile("ToyDataB.root");
  RooDataSet* ds = dynamic_cast<RooDataSet*>( datafile->Get("ToyData"));
  savedir->cd();
  gDirectory=savedir;
  
  RF->LoadDataSet(ds);
  gBenchmark->Start("moments");
  
  //Try 5 fits with different initial parameters
  RF->SetFitMethod(0); //use standard roofit
  RF->FitAndStudy(1);
  gBenchmark->Stop("moments");
  gBenchmark->Print("moments");

  datafile->Close();
  delete datafile;
  delete ds;
  delete RF;
}
