/**
	\class THSMVAPrep
	For creating TMVA training tree in THSFinalState
	Given a THSParticle it will create default branches
	from datamembers of the THSParticle class
	For each combitorial event it will then fill the tree

*/
#include "THSMVAPrep.h"
#include "TROOT.h"

////////////////////////////////////////////////////////////
/// Here I put Doxygen readable comments explaing what each function does
/// Constructor 

THSMVAPrep::THSMVAPrep(){
    // set output for TMVA
    outputName = "THSMVAClassifcation.root";
    outputFile = TFile::Open(outputName, "RECREATE");
    // get variable names
    //SetVariablesTopo_0();
    //// set dataloder and factory
    //Setup();
    //// set signal anc background events and remove NaNs
    //SetSignal();
    //SetBackground();
    //// train
    //Train();
    //if (test) Test(); 

}

/**
 * Set branches
 *
 */

// TODO : add options for different splits
// TODO : randomize split

void THSMVAPrep::SetSignal(){
    
    //if (!fTrainTree) SetTree(tree);

    std::cout<<"Preparing signals..."<<std::endl;

    gROOT->cd();
    fSignalTree = fTrainTree->CopyTree("Correct==1");

    std::cout<<"Copied tree..."<<std::endl;

    vars.reserve(nVars);

    for (UInt_t ivar=0; ivar<nVars; ivar++) fSignalTree->SetBranchAddress( variableNames[ivar], &(treevars[ivar]) );
    std::cout<<"Set branch addresses..."<<std::endl;
    for (UInt_t i=0; i<fSignalTree->GetEntries(); i++) {
        fSignalTree->GetEntry(i);
        std::cout<<treevars[0]<<std::endl;
        for (UInt_t ivar=0; ivar<nVars; ivar++) {
            if (std::isnan(treevars[ivar]))
                vars[ivar] = 0;
            else
                vars[ivar] = treevars[ivar];
        }
        
        std::cout<<vars[0]<<std::endl;
        // add training and test events; here: first half is training, second is testing
        // note that the weight can also be event-wise
        if (i < 0.9 * fSignalTree->GetEntries()) dataloader->AddSignalTrainingEvent( vars, SignalWeight );
        else                              dataloader->AddSignalTestEvent    ( vars, SignalWeight );
    }
}

void THSMVAPrep::SetBackground(){
    std::cout<<"Preparing background..."<<std::endl;
    gROOT->cd();
    fBackgroundTree = fTrainTree->CopyTree("Correct==0");

    std::cout<<"Copied tree..."<<std::endl;

    vars.reserve(nVars);
    for (UInt_t ivar=0; ivar<nVars; ivar++) fBackgroundTree->SetBranchAddress( variableNames[ivar], &(treevars[ivar]) );
    std::cout<<"Set branch adresses"<<std::endl;
    for (UInt_t i=0; i<fBackgroundTree->GetEntries(); i++) {
        fBackgroundTree->GetEntry(i);
        for (UInt_t ivar=0; ivar<nVars; ivar++) {
            if (std::isnan(treevars[ivar]))
                vars[ivar] = 0;
            else
                vars[ivar] = treevars[ivar];
        }
        // add training and test events; here: first half is training, second is testing
        // note that the weight can also be event-wise
    if (i < 0.9 * fBackgroundTree->GetEntries()) dataloader->AddBackgroundTrainingEvent( vars, BackgroundWeight );
        else                              dataloader->AddBackgroundTestEvent    ( vars, BackgroundWeight );
    }
}

/**
 * Setup the factory and dataloader before training
 *
 */

void THSMVAPrep::Setup(){

    std::cout<<"Setting up factory..."<<std::endl;

    factory = new TMVA::Factory( "THSMVAClassifcation", outputFile,"!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification" );

    dataloader = new TMVA::DataLoader("dataset");

    // added variables to dataloader
    // TODO : use mix of floats and ints?
    for (auto const& vn: variableNames) {
        dataloader->AddVariable( vn, vn, "units", 'F');
    }
}
/*
 * Select methods and train them
 *
 */

void THSMVAPrep::Train(){

    // TODO : options for preparing training
    dataloader->PrepareTrainingAndTestTree((TCut("")),"SplitMode=Random:NormMode=NumEvents:!V" );

    // TODO : add options for methods
    factory->BookMethod(dataloader, TMVA::Types::kBDT, "BDT","!H:!V:NTrees=1700:MinNodeSize=2.5%:MaxDepth=4:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20");

    //factory->BookMethod(dataloader,TMVA::Types::kRXGB, "RXGB", "!V:NRounds=160:MaxDepth=3:Eta=1");

    factory->TrainAllMethods();
}

/*
 * Test methods if selected
 *
 */

void THSMVAPrep::Test(){

    factory->TestAllMethods();
    factory->EvaluateAllMethods();
}

/*
 * Close open files etc
 * TODO : relocate close and remove function
 *
 */

void THSMVAPrep::EndPrep(){
    outputFile->Close();
}

///////////////////////////////////////////////////////////
///Destructor, here I need to delete any data members that have
///been constructed via the new operator and not been deleted elsewhere
THSMVAPrep::~THSMVAPrep(){

    delete factory;
    delete dataloader;

    delete fSignalTree;
    delete fBackgroundTree;
    delete fTrainTree;

    // TODO : destructor

}
