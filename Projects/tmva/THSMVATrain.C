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


// TODO : Datasets when using multiple topologies etc.

////////////////////////////////////////////////////////////
/// Here I put Doxygen readable comments explaing what each function does
/// Constructor 

THSMVATrain::THSMVATrain(){
    TMVA::Tools::Instance();
    // set output for TMVA
    fOutputName = "THSMVAClassification.root";
    fOutputFile = new TFile(fOutputName, "RECREATE");
    // TODO : fix file saving
}

//std::vector<Method> THSMVATrain::fMethods;
//std::vector<Split> THSMVATrain::fSplits;
std::vector<std::vector<TString>> THSMVATrain::fMVAVariables;
std::vector<std::vector<Float_t>> THSMVATrain::fMVATreeVars;


/*
 * Default training function that
 *
 */

void THSMVATrain::DefaultTrain(){

    if (!fTrainTree){
        std::cout<<"ERROR : Training tree not specified"<<std::endl;
        exit(1);
    }
    

    if (!fSplits.empty()){
        for (UInt_t i=0; i<fSplits.size(); i++) {

            //fOutputDir = fOutputFile -> mkdir(fSplits[i].GetSplitName());
            //fOutputDir->cd();

            if (fPrintTree) fTrainTree->Print();
            // TODO : make set MVA more flexible
            SetMVAVariables(fSplits[i].GetVariables()[0]); 
            
            fSplits[i].AddMVAVariables(fMVAVariables, fMVATreeVars);
            //fSplits[i].SetPointer(fTopo);
            fSplits[i].SetParticles(fSelectedParticles);

            Setup(fSplits[i].GetSplitName());

            SetSignalTree(fSplits[i].GetTreeSplit());
            SetBackgroundTree(fSplits[i].GetTreeSplit());

            EnableTest();
            Train();
            if (fTest) { Test(); }

            
        }
        EndTraining();
    }
    else{

       SetMVAVariables(); 
       
       Setup();

       SetSignalTree();
       SetBackgroundTree();

       EnableTest();
       Train();
       if (fTest) { Test(); }
       
       EndTraining();
    }
}

/**
 * Set the vectors that will be used to set the trees
 *
 */

void THSMVATrain::SetMVATreeVars(){
    
    if (fMVAVariables.empty()){
        std::cout<<"ERROR : Variables for MVA not set"<<std::endl;
        exit(1);
    }


    // TODO : catch for fSelectedParticles

    //if (fMVAVariables.size() == fTreeVarsF.size()) *fMVATreeVars = &fTreeVarsF;
    //else{
    //    for (UInt_t i=0; i<fSelectedParticles.size(); i++){
    //        fMVATreeVars.push_back(fTreeVarsF[fSelectedParticles[i]]);
    //    }
    //}
    
    
    //fMVATreeVars.resize(fMVAVariables.size() , vector<Float_t>( fMVAVariables[0].size() , 0 ) );
    fMVATreeVars.resize(fMVAVariables.size());
    for (UInt_t i=0; i<fMVATreeVars.size(); i++){
        fMVATreeVars[i].resize(fMVAVariables[i].size(), 0.0);
    }

}


/**
 * Set variables to use for training and testing with no filtering
 *
 */

// TODO : Think about having non trainable variables in fNames
void THSMVATrain::SetMVAVariables(){
    fMVAVariables = fNames;
    SetMVATreeVars();

}

/**
 * Set variables to use for training and testing with no filtering
 *
 */

void THSMVATrain::SetMVAVariables(Int_t Topology){
    GetNamesTopo(Topology);
    fSelectTopologies = true;
    fMVAVariables = fSelectNames;
    std::cout<<"Number of training particles "<<fMVAVariables.size()<<std::endl;
    SetMVATreeVars();
}

/**
 * Add a split for training
 *
 */

void THSMVATrain::AddSplit(TString inputName, TString inputVariable, Int_t * p, Int_t value){
    fTmpSplit = Split(inputName, {inputVariable}, {value});
    fTmpSplit.SetPointer({p});
    fSplits.push_back(fTmpSplit);
}

/**
 * Add a split for training
 *
 */

void THSMVATrain::AddSplit(TString inputName, std::vector<TString> inputVariables, std::vector<Int_t *> p,std::vector<Int_t> values){
    fTmpSplit = Split(inputName, inputVariables, values);
    fTmpSplit.SetPointer(p);
    fSplits.push_back(fTmpSplit);
}


// TODO : add options for different splits
// TODO : randomize split

/*
 * Set signal tree with a possible cut
 */

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


    fSignalTree->SetBranchStatus("*", kFALSE); // disable all

    fParticleCount = 0;
    fVariableCount = 0;

    // set branch addresses
    for (auto const& p : fMVAVariables){
        for (auto const& v : p){
            //fSignalTree->SetBranchAddress( v, &(fMVATreeVars[fParticleCount][fVariableCount]) );
            fSignalTree->SetBranchStatus( v, kTRUE) ;
            fVariableCount++;
        }
        fParticleCount++;
        fVariableCount=0;
    }

            
    std::cout<<"Set signal branches..."<<std::endl;

    // add signal tree to dataloader
    fDataloader->AddSignalTree( fSignalTree, fSignalWeight);
    std::cout<<"Added signal tree to data loader..."<<std::endl;
}

/*
 * Set background tree with a possible cut
 */

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
    fBackgroundTree = fTrainTree->CopyTree("Correct==0" + Filter);
    std::cout<<"Copied background tree..."<<std::endl;

    fBackgroundTree->SetBranchStatus("*", kFALSE); // disable all

    fParticleCount = 0;
    fVariableCount = 0;

    // set branch addresses
    for (auto const& p : fMVAVariables){
        for (auto const& v : p){
            //fBackgroundTree->SetBranchAddress( v, &(fMVATreeVars[fParticleCount][fVariableCount]) );
            fBackgroundTree->SetBranchStatus( v, kTRUE) ;
            fVariableCount++;
        }
        fParticleCount++;
        fVariableCount=0;
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

void THSMVATrain::Setup(TString datasetName){

    std::cout<<"Setting up factory..."<<std::endl;

    fFactory = new TMVA::Factory( "THSMVAClassification", fOutputFile,"!V:!Silent:Color:DrawProgressBar:Transformations=I:AnalysisType=Classification" );

    std::cout<<"Setting up dataloader..."<<std::endl;

    fDatasetName = "dataset" + datasetName;
    fDataloader = new TMVA::DataLoader(fDatasetName);

    // added variables to dataloader
    // TODO : use mix of floats and ints?
    for (auto const& p : fMVAVariables){
        for (auto const& vn: p){
            fDataloader->AddVariable( vn, vn, "units", 'F');
        }
    }
}

/*
 * Add method to be used for training
 *
 */

void THSMVATrain::AddMethod(Method method){
    fMethods.push_back(method);
}

/*
 * Select methods and train
 *
 */

void THSMVATrain::Train(){

    fOutputFile->cd();

    // TODO : options for preparing training
    fDataloader->PrepareTrainingAndTestTree((TCut("")),"SplitMode=Random:NormMode=NumEvents:!V" );

    if (fMethods.empty()){
        std::cout<<"No methods provided..."<<std::endl;
        std::cout<<"Using default method..."<<std::endl;
        fFactory->BookMethod(fDataloader, TMVA::Types::kBDT, "BDT","!H:!V:NTrees=500:MinNodeSize=2.5%:MaxDepth=2:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20");
    }
    else{
        for (auto const& m : fMethods){
            fFactory->BookMethod(fDataloader, m.GetType(), m.GetName(), m.GetParameters());
        }

    }

    fFactory->TrainAllMethods();
}

/*
 * Test methods if selected
 *
 */

void THSMVATrain::Test(){

    fOutputFile->cd();

    fFactory->TestAllMethods();
    fFactory->EvaluateAllMethods();
}

/*
 * Close open files etc
 * TODO : relocate close and remove function
 *
 */

void THSMVATrain::EndTraining(){
    //fTrainTree->SetDirectory(fOutputFile);
    fOutputFile->Close();
}

/*
 * Write a copy of THSMVA instance used for training
 *
 */

void THSMVATrain::WriteTHSMVA(TString name){

    fMVA.SetMethods(GetMethods());
    fMVA.SetSplits(GetSplits());

    fMVA.SetName(name);
    fMVA.Write();
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
