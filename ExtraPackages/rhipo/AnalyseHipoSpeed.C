//root $RHIPO/Hipo2Root.C $RHIPO/ConvertSomeBanks.C file1.hipo file2.hipo ...
{
  //Edit this macro to only convert the banks selected with ConfigBank
  THipo Hipo;
  Hipo.AddCommandLineFiles();
  Hipo.ConfigBank("RECHB::Particle");
  Hipo.ConfigBank("RECHB::Detector");
  Hipo.ConfigBank("FT::particles");

  //Get the necessary items from Particle Bank
  THipoBank* bank1=Hipo.GetBank("RECHB::Particle");
  // THipoItem *px=bank1->GetItem("px");
  // THipoItem *py=bank1->GetItem("py");
  // THipoItem *pz=bank1->GetItem("pz");
  // THipoItem *vx=bank1->GetItem("vx");
  // THipoItem *vy=bank1->GetItem("vy");
  // THipoItem *vz=bank1->GetItem("vz");
  THipoItem *pid=bank1->GetItem("pid");
  //  THipoItem *mass=bank1->GetItem("mass");
  //Get the necessary items from the Detector bank
  // THipoBank* bank2=Hipo.GetBank("REC::Detector");
  // THipoItem *d_index=bank2->GetItem("index");
  // THipoItem *d_pindex=bank2->GetItem("pindex");
  // THipoItem *d_time=bank2->GetItem("time");
  // THipoItem *d_det=bank2->GetItem("detector");

  gBenchmark->Start("time");
   std::clock_t start;
    double duration;
    start = std::clock();

  //Declare some variables we might want to use
  Int_t counter=0;
  while(Hipo.NextFile())//loop over input files
    while(Hipo.NextEvent()){ //loop over events
      //      while(bank1->NextEntry()){ //loop over particles in event
      counter++;
      //}
    }
  cout<<"counter "<<counter<<endl;
   duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
   cout<<"duration "<<duration<<endl;
  gBenchmark->Stop("time");
  gBenchmark->Print("time");
  
  //finished exit here so it doesn't try to process hipo files!
  gROOT->ProcessLine(".q");
}
