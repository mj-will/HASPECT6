//root --hsdata --hsfinal=THSK0L RunFSRootK0L.C
//You need to replace K0L with your final state class name
{
  //Create FinalState
  THSK0L* fs=new THSK0L();
  // enable training
  fs->SetTrain(kTRUE);
  //fs->SetGenerated(); //just analyse generated branch
   fs->SetMaxParticles(10);
  //create datamanager
  THSDataManager* dm=new THSDataManager();
  TChain chain("HSParticles");
  chain.Add("/w/work1/jlab/hallb/g13/K0L18/K0LSim_11_23/DSTJob14659644.farmpbs14.root");
  chain.Add("/w/work1/jlab/hallb/g13/K0L18/K0LSim_11_23/DSTJob14659445.farmpbs14.root");
  chain.Add("/w/work1/jlab/hallb/g13/K0L18/K0LSim_11_23/DSTJob14659446.farmpbs14.root");
  chain.Add("/w/work1/jlab/hallb/g13/K0L18/runs_3.3_1.3/Run_054044.root");
  chain.Add("/w/work1/jlab/hallb/g13/K0L18/runs_3.3_1.3/Run_054047.root");
  dm->InitChain(&chain);
  //connect Project to HSParticles
  fs->SetDataManager(dm);
  Int_t counter=0;
  
  //create ouput tree
  TFile* outfile=new TFile("OUTPUTRec.root","recreate");
  TTree* outtree=new TTree("FinalTree","output tree");
  //fs->FinalStateOutTree(outtree); //connect ouput tree to project branches
  fs->TMVAOutTree(outtree); //connect ouput tree to project branches

  // set maxmium number of events
  fs->SetNEvents(-1,50000,10000);
  
  gBenchmark->Start("timer");
  while(dm->ReadEvent()){
    //loop over events
    fs->ProcessEvent();
      if (counter%100000 == 0){
          std::cout<<"Event count: "<<counter<<" "<<std::endl;
          // check number of signals generated
          if (fs->CheckSignalCount(outtree)) break;
          std::cout<<" "<<std::endl;
      }
      counter++;
  }

  std::cout<<" "<<std::endl;

  gBenchmark->Stop("timer");
  gBenchmark->Print("timer");
  // run the training 
  fs->RunTraining();
  outtree->GetDirectory()->cd();
  // write the THSMVA class used for this instance of training
  // this will be used later for configuration of application
  fs->WriteConfig("Setup");

  outfile->cd();
  outtree->Write();
  delete outfile;
}
