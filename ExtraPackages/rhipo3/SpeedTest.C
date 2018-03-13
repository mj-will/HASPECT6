//root $RHIPO/Hipo2Root.C $RHIPO/SpeedTest.C file1.hipo file2.hipo ...
{
  //Edit this macro to only convert the banks selected with ConfigBank
  THipo Hipo;
  //Hipo.AddCommandLineFiles();
  Hipo.SetCompression(1);

  Hipo.ToRoot(); //process files given on command line

  
  Hipo.InitFile("/home/dglazier/clas12data/out_clas_002835.evio.10.hipo");
  // Hipo.ConfigBanks();
  Hipo.ConfigOnlyBank("REC::Event");
  Hipo.ConfigOnlyBank("REC::Particle");
  Hipo.ConfigBanks();
   
  gBenchmark->Start("timer");
 
  while(Hipo.NextEvent());
  gBenchmark->Stop("timer");
  gBenchmark->Print("timer");

  //finished exit here so it doesn't try to process hipo files!
  gROOT->ProcessLine(".q");
}
