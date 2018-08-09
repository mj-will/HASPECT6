//root --hsdata --hsfinal=THS2pi RunFSRoot2pi.C
//You need to replace 2pi with your final state class name
{
  //Create FinalState
  THS2pi* fs=new THS2pi();
  fs->SetTrain(kTRUE);
  // fs->SetGenerated(); //just analyse generated branch
   fs->SetMaxParticles(10);
  //create datamanager
  THSDataManager* dm=new THSDataManager();
  TChain chain("HSParticles");
  chain.Add("/w/work1/jlab/hallb/clas12/tmva_gemc_data/rho10_tm1_sm1_5.3.3/out_*root");
  dm->InitChain(&chain);
  //connect Project to HSParticles
  fs->SetDataManager(dm);
  Int_t counter=0;
  
  //create ouput tree
  TFile* outfile=new TFile("TestTrain.root","recreate");
  TTree* outtree=new TTree("THSMVATree","output tree");
  outtree->SetAutoSave(1E4);
  //  fs->FinalStateOutTree(outtree); //connect ouput tree to project branches
  fs->TMVAOutTree(outtree); //connect ouput tree to project branches
  
  gBenchmark->Start("timer");

  fs->SetNEvents(50000,1000);
   
  while(dm->ReadEvent()){//loop over events
    fs->ProcessEvent();
    if (counter%1000 == 0){
        if (fs->CheckSignalCount(outtree)) break;
    }
    counter++;
    //if (counter >= 50000) break;
  }
    

  gBenchmark->Stop("timer");
  gBenchmark->Print("timer");
 
  fs->RunTraining();
  outtree->GetDirectory()->cd();
  fs->WriteConfig("Setup");

  //fs->RunApp();

  outfile->cd();
  outtree->Write();

  delete outfile;
}
