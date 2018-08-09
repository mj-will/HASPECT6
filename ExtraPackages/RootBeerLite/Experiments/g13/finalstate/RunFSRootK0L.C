//root --hsdata --hsfinal=THSK0L RunFSRootK0L.C
//You need to replace K0L with your final state class name
{
  //Create FinalState
  THSK0L* fs=new THSK0L();
  //fs->SetGenerated(); //just analyse generated branch
   fs->SetMaxParticles(10);
  //create datamanager
  THSDataManager* dm=new THSDataManager();
  TChain chain("HSParticles");
  //  chain.Add("/w/work1/jlab/hallb/g13/test13/Run_054044.root");
  chain.Add("/w/work1/jlab/hallb/g13/K0L18/K0LSim_11_23/DSTJob14659644.farmpbs14.root");
  dm->InitChain(&chain);
  //connect Project to HSParticles
  fs->SetDataManager(dm);
  Int_t counter=0;
  
  //create ouput tree
  TFile* outfile=new TFile("OUTPUTRec.root","recreate");
  TTree* outtree=new TTree("FinalTree","output tree");
  fs->FinalStateOutTree(outtree); //connect ouput tree to project branches
  
  gBenchmark->Start("timer");
  
  while(dm->ReadEvent()){//loop over events
    fs->ProcessEvent();
  }
  gBenchmark->Stop("timer");
  gBenchmark->Print("timer");
  
  outfile->cd();
  outtree->Write();
  delete outfile;
}
