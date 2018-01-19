///////////////////////////////////////////////////
///run with
/// set the file name to the GenerateToys directory e.g.
/// setenv FILEDIR $PWD/Toys_A
/// 1 off create data root --hsfit --RooHSSphHarMoments.cxx CreateModel.C
/// Set an extra directory to put output into e.g.
/// setenv OUTDIR Run1MCMC
/// The fitting script e.g.
/// setenv RUNMAC $PWD/FitToysMCMC.C
/// setenv CREATEMODEL $PWD/CreateModel.C
/// setenv LAUNCH $PWD
/// The first argument=number toy fits, last arguemnt =1 ->qsub 0 =>run local
/// root 'HSSubmitToys.C(1,1)'
{

  gROOT->LoadMacro("CreateModel.C");
  RF->SetOutDir("./");
  
  //Get data for fitting
  TDirectory* savedir=gDirectory;
  TFile* datafile=new TFile("ToyData.root");
  RooDataSet* ds = dynamic_cast<RooDataSet*>( datafile->Get("ToyData"));
  savedir->cd();
  gDirectory=savedir;

  RF->LoadDataSet(ds);
  
  
  //Try 5 fits with different initial parameters
  RF->SetFitMethod(2); //use standard roofit
  RF->SetNMCMC(10000);
  RF->FitAndStudy(0);
  
  datafile->Close();
  delete datafile;
  delete ds;
  
}
