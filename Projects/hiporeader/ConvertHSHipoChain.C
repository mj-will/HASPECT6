//Run with root --hsdata ConvertHSHipoChain.C 
{
  THSHipoReader* reader=new THSHipoReader();
  reader->SetWriteGenBranch("Generated");

  TChain chain("hipo");
  chain.Add(HSin()+"out*.hipo");
  
  gBenchmark->Start("time");

  reader->ReadWriteChain(&chain,HSout(),".root");
  
  cout<<"Analysed "<<reader->GetEntry()<<" events"<<endl;
  
  gBenchmark->Stop("time");
  gBenchmark->Print("time");

  // //loop through tree and add a UID, 
  // reader->SetUID(100); //may want to apply offset
  // reader->AddUID(HSout());
}
