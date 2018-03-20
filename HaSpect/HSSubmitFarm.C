///////////////////////////////////////////////////
///run with
/// job submision script (ie script which will run on farm) 
/// e.g. setenv HS_FARMRUN $PWD/pbs_run
/// job submission type  (qsub, jsub,...)
/// e.g. setenv HS_FARMSUB qsub
/// The first arguemnt  number of farm jobs
/// The last argument =1 ->qsub 0 =>run local
/// root 'HSSubmitFarm.C(1,1)'
void HSSubmitFarm(Int_t Njobs,Int_t toFarm=1){

  //Look for env variable FARMRUN for job submission script 
  TString farmrun;
  if(gSystem->Getenv("HS_FARMRUN"))
    farmrun=gSystem->Getenv("HS_FARMRUN");
  else
    farmrun="./pbs_run";
  //Look for variable FARMSUBMIT
  TString farmsub;
  if(gSystem->Getenv("HS_FARMSUBMIT"))
    farmsub=gSystem->Getenv("HS_FARMSUBMIT");
  else
    farmsub="qsub";

  
  //create a farm job for each toy requested    
  gSystem->Setenv("LAUNCH",TString(gSystem->Getenv("PWD")));
   for(Int_t i=0;i<Njobs;i++){
    TString JobName=Form("HSFarm_%d",i);
    cout<<"sending JobName "<<JobName<< endl;
    gSystem->Setenv("HS_JOBNAME",JobName);
    if(toFarm)
      gSystem->Exec(farmsub+" "+farmrun);
    else
      gSystem->Exec(farmrun);
    //gSystem->Exec("sleep 0.2");
   
  }
  
}
 
