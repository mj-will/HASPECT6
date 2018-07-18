/**
	\class THSMVATrain
	For creating TMVA training tree in THSFinalState
	Given a THSParticle it will create default branches
	from datamembers of the THSParticle class
	For each combitorial event it will then fill the tree

*/
#include "THSMVATrain.h"
#include "TROOT.h"


ClassImp(THSMVATrain);


////////////////////////////////////////////////////////////
/// Here I put Doxygen readable comments explaing what each function does
/// Constructor 

THSMVATrain::THSMVATrain(){
    TMVA::Tools::Instance();
    // set output for TMVA
    fOutputName = "THSMVAClassifcation.root";
    fOutputFile = TFile::Open(fOutputName, "RECREATE");

}


void THSMVATrain::DefaultTrain(){

    if (!fTrainTree){
        std::cout<<"ERROR : Training tree not specified"<<std::endl;
        exit(1);
    }

    SetMVAVariables(2); 
    
    Setup();

    SetSignalTree();
    SetBackgroundTree();

    Train();
    EnableTest();
    if (fTest) { Test(); }
    
    EndTraining();
}

/**
 * Set the vectors that will be used to set the trees
 *
 * Assumes all particles have same number of variables
 */

void THSMVATrain::SetMVATreeVars(){
    
    if (fMVAVariables.empty()){
        std::cout<<"ERROR : Variables for MVA not set"<<std::endl;
        exit(1);
    }
    
    fMVATreeVars.resize(fMVAVariables.size() , vector<Float_t>( fMVAVariables[0].size() , 0 ) );

}


/**
 * Set variables to use for training and testing with no filtering
 *
 */

// TODO : Think about having non trainable variables added in fNames
void THSMVATrain::SetMVAVariables(){
    fMVAVariables = fNames;
    SetMVATreeVars();

}

void THSMVATrain::SetMVAVariables(Int_t Topology){
    GetNamesTopo(Topology);
    fSelectTopologies = true;
    fMVAVariables = fSelectNames;
    SetMVATreeVars();
}

// TODO : add options for different splits
// TODO : randomize split


void THSMVATrain::SetSignalTree(TString Filter){

    std::cout<<"Setting signal tree..."<<std::endl;

    if (!fTrainTree){
        std::cout<<"ERROR : Training tree not set..."<<std::endl;
        std::cout<<"Exiting..."<<std::endl;
        exit(1);
    }

    if (fMVATreeVars.empty()){
        std::cout<<"ERROR : Variables for filling tree not set"<<std::endl;
        std::cout<<"Exiting..."<<std::endl;
        exit(1);
    }

    // copy selection of fTrainTree
    fSignalTree = fTrainTree->CopyTree("Correct==1" + Filter);
    std::cout<<"Copied signal tree..."<<std::endl;

    fParticleCount = 0;
    fVariableCount = 0;

    // set branch addresses
    for (auto const& p : fMVAVariables){
        for (auto const& v : p){
            fSignalTree->SetBranchAddress( v, &(fMVATreeVars[fParticleCount][fVariableCount]) );
            fVariableCount++;
        }
        fParticleCount++;
    }
            
    std::cout<<"Set signal branches..."<<std::endl;

    // add signal tree to dataloader
    fDataloader->AddSignalTree( fSignalTree, fSignalWeight);
    std::cout<<"Added signal tree to data loader..."<<std::endl;
}


void THSMVATrain::SetBackgroundTree(TString Filter){

    std::cout<<"Setting background tree..."<<std::endl;

    if (!fTrainTree){
        std::cout<<"ERROR : Training tree not set..."<<std::endl;
        std::cout<<"Exiting..."<<std::endl;
        exit(1);
    }

    if (fMVATreeVars.empty()){
        std::cout<<"ERROR : Variables for filling tree not set"<<std::endl;
        std::cout<<"Exiting..."<<std::endl;
        exit(1);
    }

    // copy selection of fTrainTree
    fBackgroundTree = fTrainTree->CopyTree("Correct==1" + Filter);
    std::cout<<"Copied background tree..."<<std::endl;

    fParticleCount = 0;
    fVariableCount = 0;

    // set branch addresses
    for (auto const& p : fMVAVariables){
        for (auto const& v : p){
            fBackgroundTree->SetBranchAddress( v, &(fMVATreeVars[fParticleCount][fVariableCount]) );
            fVariableCount++;
        }
        fParticleCount++;
    }
            
    std::cout<<"Set background branches..."<<std::endl;

    // add background tree to dataloader
    fDataloader->AddBackgroundTree( fBackgroundTree, fBackgroundWeight);
    std::cout<<"Added background tree to data loader..."<<std::endl;
}


/**
 * Setup the factory and dataloader before training
 *
 */

void THSMVATrain::Setup(){

    std::cout<<"Setting up factory..."<<std::endl;

    fFactory = new TMVA::Factory( "THSMVAClassifcation", fOutputFile,"!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification" );

    std::cout<<"Setting up dataloader..."<<std::endl;

    fDataloader = new TMVA::DataLoader("dataset");

    // added variables to dataloader
    // TODO : use mix of floats and ints?
    for (auto const& p : fMVAVariables){
        for (auto const& vn: p){
            fDataloader->AddVariable( vn, vn, "units", 'F');
        }
    }
}


void THSMVATrain::AddMethod(Method method){
    fMethods.push_back(method);
}

/*
 * Select methods and train them
 *
 */

void THSMVATrain::Train(){

    // TODO : options for preparing training
    fDataloader->PrepareTrainingAndTestTree((TCut("")),"SplitMode=Random:NormMode=NumEvents:!V" );

    if (fMethods.empty()){
        std::cout<<"No methods provided..."<<std::endl;
        std::cout<<"Using default method..."<<std::endl;
        fFactory->BookMethod(fDataloader, TMVA::Types::kBDT, "BDT","!H:!V:NTrees=500:MinNodeSize=2.5%:MaxDepth=2:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20");
    }

    // TODO : add options for methods
    //fFactory->BookMethod(fDataloader, TMVA::Types::kBDT, "BDT","!H:!V:NTrees=1700:MinNodeSize=2.5%:MaxDepth=4:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20");

        //fFactory->BookMethod(fDataloader,TMVA::Types::kRXGB, "RXGB", "!V:NRounds=160:MaxDepth=3:Eta=1");
        //
    else{
        for (auto const& m : fMethods){
            fFactory->BookMethod(fDataloader, m.type, m.name, m.parameters);
        }

    }
    fFactory->TrainAllMethods();
}

/*
 * Test methods if selected
 *
 */

void THSMVATrain::Test(){

    fFactory->TestAllMethods();
    fFactory->EvaluateAllMethods();
}

/*
 * Close open files etc
 * TODO : relocate close and remove function
 *
 */

void THSMVATrain::EndTraining(){
    fOutputFile->Close();
}

///////////////////////////////////////////////////////////
///Destructor, here I need to delete any data members that have
///been constructed via the new operator and not been deleted elsewhere
THSMVATrain::~THSMVATrain(){

    delete fFactory;
    delete fDataloader;

    delete fSignalTree;
    delete fBackgroundTree;

    delete fOutputFile;
    delete fTreeOutputFile;

    // TODO : destructor

}
