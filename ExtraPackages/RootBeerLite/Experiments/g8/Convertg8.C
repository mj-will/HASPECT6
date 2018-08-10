//Run with  root --rootbeer $G8/RootSetup.C --THSCLASg8.C --hsin=testruns.txt --hsout=/w/work0/home/dglazier/test8/ --Edge=XXX Convertg8.C
{
  //loog for edge position
  Int_t	 EDGEVAL=0;	
 
  for(Int_t i=1;i<gApplication->Argc();i++){
    TString opt=gApplication->Argv(i);
    //look for --proof=Nworkers optionif Nworkers not given all cores will be used
    if((opt.Contains("--Edge"))) EDGEVAL=(TString(opt(7,opt.Sizeof())).Atoi());
  }

  THSCLASg8* reader=new THSCLASg8();
  reader->SetEdge(EDGEVAL);
  reader->SetOutDir(HSout());

  //Fake chain just using the Add and GetListofFiles
  TChain chain("dst");
  //Give list of files
  TFileCollection fc("dum","",HSinFile());
  chain.AddFileInfoList(fc.GetList());
  
  gBenchmark->Start("time");

  reader->InitChain(&chain);
  reader->WriteParticles();
  cout<<"Analysed "<<reader->GetEntry()<<" events"<<endl;
  
  gBenchmark->Stop("time");
  gBenchmark->Print("time");
 
}
