///////////////////////////////////////////////////
///run with
/// setenv HS_LAUNCH  $PWD
/// set the file name to the GenerateToys directory e.g.
/// setenv HS_FILEDIR $PWD/Toys_GenFit
/// Set an extra directory to put output into e.g.
/// setenv HS_OUTDIR Run1
/// The fitting script e.g.
/// setenv HS_RUNMAC $PWD/GenFitToys.C
/// setenv HS_GENMODEL $PWD/CreateGenModel.C
/// setenv HS_FITMODEL $PWD/CreateFitModel.C
/// setenv HS_FARMRUN $PWD/pbs_genfit
/// job submission type  (qsub, jsub,...)
/// setenv HS_FARMSUB qsub
/// The first argument=number toy fits, last arguemnt =1 ->qsub 0 =>run local
/// root 'HSSubmitFarm.C(1,0)'
/// When all jobs finished
/// root --hsfit 'MergeResults.C(Njobs)'
{
  RooRandom::randomGenerator()->SetSeed(0);//=>diffeent random seed each time
  //Generate data for fitting
  //Load generate model
  gROOT->LoadMacro(gSystem->Getenv("HS_GENMODEL"));
  Long64_t nexp=RooRandom::randomGenerator()->Poisson(HS::gRF->GetModel()->expectedEvents(HS::gRF->GetVariables()));
  RooDataSet* ds=HS::gRF->GetModel()->generate(HS::gRF->GetVariables(),nexp);
  

  //Now switch to fit model
  gROOT->LoadMacro(gSystem->Getenv("HS_FITMODEL"));

  HS::gRF->SetOutDir("./");
  


  //Do sweights fits
  THSWeights* lastWeights=nullptr;
  THSsPlot* sp=nullptr;
  for(Int_t isp=0;isp<gsPlots->GetEntries();isp++){
    //By looping over sFits we can do factorised subtractions
    //Users just hace to add the splot to gsPlots when they contruct it
    sp= dynamic_cast<THSsPlot*>(gsPlots->At(isp));
     //load new data
    sp->LoadDataSet(ds);
    //do splot fit
    sp->SetSaveWeights(kFALSE);
    if(lastWeights) {
      sp->LoadWeights(lastWeights);
      sp->SetWeightName(sp->GetTitle());
      sp->SetDataWeightFast();
    }
    //sp->SetPlot(kFALSE);
    sp->Fit();
    sp->sPlot();
    sp->SavePlots(TString("sPlots")+sp->GetTitle()+".root");;

    //keep weights to add to next factorised sPlot
    if(lastWeights) {delete lastWeights;lastWeights=nullptr;}
    lastWeights=sp->GetWeights();

  }
  lastWeights->PrintWeight();
  
  HS::gRF->LoadDataSet(ds);
  //add splot weights
  if(lastWeights) HS::gRF->LoadWeights(lastWeights);
  HS::gRF->SetWeightName("sig");
  HS::gRF->SetDataWeightFast();
 //Try 1 fits with different initial parameters
  HS::gRF->SetFitMethod(0); //use standard roofit
  HS::gRF->FitAndStudy(1);

  delete ds;
  delete HS::gRF;HS::gRF=nullptr;
}
