//Run with root  --rootbeer $G13/RootSetup.C --THSCLASg13.C Convertg13Settings.C  --hsin=infilelist --hsout=outdir
{

  THSCLASg13* reader=new THSCLASg13();
  reader->SetOutDir(HSout());
  reader->SetWriteGenBranch("Generated");
  reader->SetSim();

  //Fake chain just using the Add and GetListofFiles
  TChain chain("dst");
  //Give list of files
  //TFileCollection fc("dum","","testruns.txt");
  //TFileCollection fc("dum","",HSinFile());
  chain.Add(HSin()+"*.root");
  
  gBenchmark->Start("time");

  reader->InitChain(&chain);
  reader->WriteParticles();
  cout<<"Analysed "<<reader->GetEntry()<<" events"<<endl;
  
  gBenchmark->Stop("time");
  gBenchmark->Print("time");
 
}
