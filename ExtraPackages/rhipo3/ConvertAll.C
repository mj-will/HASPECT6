{
  THipo Hipo;
  Hipo.AddCommandLineFiles();
  Hipo.SetCompression(1);
  gBenchmark->Start("time");
  Hipo.ToRoot(); //process files given on command line
  gBenchmark->Stop("time");
  gBenchmark->Print("time");
  
  //finished exit here so it doesn't try to process hipo files!
  gROOT->ProcessLine(".q");
}
