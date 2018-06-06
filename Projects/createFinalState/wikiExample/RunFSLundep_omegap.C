//root --hsdata --hsfinal=THSep_omegap RunFSLund.C
//You need to replace ep_omegap with your final state class name
{
  //Create FinalState
  THSep_omegap* fs=new THSep_omegap();
  //create datamanager
  THSLundReader* dm=new THSLundReader();
  dm->Init("Epto_omega_0.ld","HSParticles");
  fs->SetDataManager(dm);
  Int_t counter=0;
  
  //create ouput tree
  TFile* outfile=new TFile("gentest.root","recreate");
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
