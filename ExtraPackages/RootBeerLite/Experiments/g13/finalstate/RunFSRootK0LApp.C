//root --hsdata --hsfinal=THS2pi RunFSRoot2piApp.C
//You need to replace 2pi with your final state class name
{
    //Create FinalState
    THSK0L* fs=new THSK0L();
    fs->SetTrain(kFALSE);
    // fs->SetGenerated(); //just analyse generated branch
    fs->SetMaxParticles(10);
    //create datamanager
    THSDataManager* dm=new THSDataManager();
    TChain chain("HSParticles");
    chain.Add("/w/work1/jlab/hallb/g13/K0L18/runs_3.3_1.3/Run_054044.root");
    //chain.Add("/w/work1/jlab/hallb/g13/K0L18/K0LSim_11_23/DSTJob14659644.farmpbs14.root");
    dm->InitChain(&chain);
    //connect Project to HSParticles
    fs->SetDataManager(dm);
    Int_t counter=0;

    //create ouput tree
    TFile* outfile=new TFile("TestApp.root","recreate");
    TTree* outtree=new TTree("OutputTree","output tree");
    outtree->SetAutoSave(1E5);
    fs->FinalStateOutTree(outtree); //connect ouput tree to project branches
    //fs->TMVAOutTree(outtree); //connect ouput tree to project branches

    TFile* setupFile = new TFile("OUTPUTRec.root","LOAD");
    THSMVA* setup; setupFile->GetObject("Setup;1", setup);

    fs->SetApplication(setup);

    gBenchmark->Start("timer");

    while(dm->ReadEvent()){//loop over events
        fs->ProcessEvent();
        if (counter == 50000){
            //break;
        }
        counter++;
    }

    gBenchmark->Stop("timer");
    gBenchmark->Print("timer");

    fs->EndApplication(outfile);

    outfile->cd();
    outtree->Write();

    delete outfile;
}
