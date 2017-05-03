//root $RHIPO/Hipo2Root.C $RHIPO/ConvertSomeBanks.C file1.hipo file2.hipo ...
{
  //Edit this macro to only convert the banks selected with ConfigBank
  THipo Hipo;
  Hipo.AddCommandLineFiles();
  Hipo.ConfigBank("RECHB::Particle");
  //  Hipo.ConfigBank("RECHB::Detector>>time:energy");

  Hipo.ToRoot(); //process files given on command line

  //finished exit here so it doesn't try to process hipo files!
  gROOT->ProcessLine(".q");
}
