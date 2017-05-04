//Run with root --hsdata ConvertHS.C --hsin=ep_eppi0.lund --hsout=ep_eppi0.root
{
  THSHipoReader* reader=new THSHipoReader();
  // hipo->SetWriteGenBranch("Generated");

  gBenchmark->Start("time");

  reader->Init(HSin());
  reader->WriteParticles(HSout());

  gBenchmark->Stop("time");
  gBenchmark->Print("time");

  //loop through tree and add a UID, 
  //reader->SetUID(100); //may want to apply offset
  //reader->AddUID(HSout());
  gROOT->ProcessLine(".q");
}
