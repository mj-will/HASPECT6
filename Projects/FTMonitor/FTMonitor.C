///Run with root --hsdata FTMonitor.C.C --hsin=/path/to/hipo/files --hsout=/path/to/output/root/files
{
  THSHipoFTMon* reader=new THSHipoFTMon();
  reader->SetUseUnGated(); //TEMP HACK!!!!
  reader->SetTrigPeriod(0); //0=Enginnering; 1=RG-A 02/18;...
  

  ///////////////////////////////////////////////////////////////////
  ////Set input files via TChain (i.e. can use wildcards etc.)
  TChain chain("hipo");
  chain.Add(HSin()+"/*.hipo");

  ///////////////////////////////////////////////////////////////////
  //Create output directory and make sure it is OK to continue of it
  //already exists
  if(gSystem->MakeDirectory(HSout().Data())==-1){
    cout<<"Warning directory "<<HSout()<<" already exists, is it OK to continue? y/n"<<endl;
    TString ans;
    cin>>ans;
    if(ans!=TString("y"))
      gROOT->ProcessLine(".q"); 
  }
  //////////////////////////////////////////////////////////////////
  
  gBenchmark->Start("time");
  //////////////////////////////////////////////////////////////////
  //Analyse all files in chain
  reader->ReadWriteChain(&chain,HSout(),".root");
  
  cout<<"Analysed "<<reader->GetEntry()<<" events"<<endl;
  
  gBenchmark->Stop("time");
  gBenchmark->Print("time");

 }
