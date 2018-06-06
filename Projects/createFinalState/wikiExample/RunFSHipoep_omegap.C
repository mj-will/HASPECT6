//root --hsdata --hsfinal=THSep_omegap RunFSHipoep_omegap.C
//You need to replace ep_omegap with your final state class name
{
  //Create FinalState
  THSep_omegap* fs=new THSep_omegap();
   // fs->SetGenerated(); //just analyse generated branch
  //create datamanager
  THSHipoTrigger* dm=new THSHipoTrigger();
  TChain chain("HSParticles");
  chain.Add("/home/dglazier/clas12data/out_clas_003432.evio.0.hipo");
  dm->InitChain(&chain);
  //connect Project to HSParticles
  fs->SetDataManager(dm);
  Int_t counter=0;
  
  //create ouput tree
  TFile* outfile=new TFile("OUTPUTHipo.root","recreate");
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
