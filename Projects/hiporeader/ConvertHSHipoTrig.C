//Run with root --hsdata ConvertHSHipoTrig.C --hsin=ep_eppi0.hipo --hsout=ep_eppi0.root
{
  THSHipoTrigger* reader=new THSHipoTrigger();
  // hipo->SetWriteGenBranch("Generated");

  gBenchmark->Start("time");

  reader->Init(HSin());
  reader->WriteParticles(HSout());

  cout<<"Analysed "<<reader->GetEntry()<<" events"<<endl;
  
  gBenchmark->Stop("time");
  gBenchmark->Print("time");

  //loop through tree and add a UID, 
  reader->SetUID(100); //may want to apply offset
  reader->AddUID(HSout());
}
