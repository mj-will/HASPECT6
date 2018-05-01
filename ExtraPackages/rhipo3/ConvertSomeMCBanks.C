//root $RHIPO/Hipo2Root.C $RHIPO/ConvertSomeMCBanks.C file1.hipo file2.hipo ...
{
  //Edit this macro to only convert the banks selected with ConfigBank
  THipo Hipo;
  Hipo.AddCommandLineFiles();
  Hipo.SetCompression(1);
  Hipo.ConfigOnlyBank("REC::");
  Hipo.ConfigOnlyBank("RUN::config");
  Hipo.ConfigOnlyBank("RAW::scaler");
  Hipo.ConfigOnlyBank("FT::particles");
  Hipo.ConfigOnlyBank("FTCAL::clusters");
  Hipo.ConfigOnlyBank("MC::");


  //Hipo.ConfigOnlyBank("REC::Event");
  //Hipo.ConfigOnlyBank("RUN");

  Hipo.ToRoot(); //process files given on command line

  //finished exit here so it doesn't try to process hipo files!
  gROOT->ProcessLine(".q");
}
