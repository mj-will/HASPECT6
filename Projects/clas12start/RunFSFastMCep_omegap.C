//root --hsdata --hsfinal=THSep_omegap RunFSFastMCep_omegap.C
//You need to replace ep_omegap with your final state class name
{
  //Create FinalState
  THSep_omegap* fs=new THSep_omegap();
  //fs->SetGenerated(); 
  //create datamanager
  THSCLAS12FastMC* dm=new THSCLAS12FastMC();
  dm->SetTorCurrent(-3770);
  dm->SetWriteGenBranch("Generated");
  dm->Init("genAsym/AsymPhiEpto_omega_0.ld","HSParticles");
  //connect Project to HSParticles
  fs->SetDetParts(dm->GetParticles());
  fs->SetGenParts(dm->GetGenerated());
  Int_t counter=0;
  
  //create ouput tree
  TFile* outfile=new TFile("rectestCh.root","recreate");
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
