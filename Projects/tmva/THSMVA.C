/**
	\class THSMVA
	Base class for MVA analysis within THSFinalState
	Derived classes THSMVAPrep and THSMVAApp are responsible
	for preparing training tree and getting the reader response respectively
	This class inherits from TObject so can be saved to root file
	and retrieved later
*/


#include "THSMVA.h"
//#include "isnan"

ClassImp(THSMVA);


////////////////////////////////////////////////////////////
/// Here I put Doxygen readable comments explaing what each function does

THSMVA::THSMVA(){

}

// Initialize static variables
std::vector<std::vector<TString>> THSMVA::fNames;
std::vector<std::vector<Float_t>> THSMVA::fTreeVarsF;

// TODO : AddParticles, AddVariables, SetParticles, SetVariables

/**
 * Set the names for all the particle/variable combinations
 * 
 *
 */

void THSMVA::SetNames() {

    if (fNames.size()) {
        std::cout<<"ERROR: Names already set"<<std::endl;
        for (auto const& v : fNames){
            for (auto const& n : v){
                std::cout<<n<<" ";
            }
        }
        std::cout<<""<<std::endl;
        exit(1);
    }

    std::cout<<"Setting up variables..."<<std::endl;

    fNames.resize(fParticleID.size());
    fTreeVarsF.resize(fParticleID.size());
    fTreeVarsI.resize(fParticleID.size());

    //std::cout<<"Starting loop..."<<std::endl;

    fVariableCount = 0;
    fParticleCount = 0;

    for (auto const& p : fParticleID) {
        for (auto const& v : fVariableID) {
            fNames[fParticleCount].push_back(p+v);
            //std::cout<<"Added name..."<<std::endl;
            if (fTypes[fVariableCount] == "F") fTreeVarsF[fParticleCount].push_back(0);
            if (fTypes[fVariableCount] == "I") fTreeVarsI[fParticleCount].push_back(0);
            fVariableCount++;
        }
        fVariableCount = 0;
        fParticleCount++;
    }

    std::cout<<"Printing sizes..."<<std::endl;
    std::cout<<fTreeVarsF.size()<<" / "<<fTreeVarsF[0].size()<<std::endl;
}


/**
 * Set the topologies in which the particles are present
 *
 */

void THSMVA::SetTopologies() {
    std::cout<<"Setting Topologies..."<<std::endl;
     
    if (fParticleTopologies.empty()) {
        fParticleTopologies.resize(fParticleID.size());
        fParticleCount = 0;
        for (auto const& particle : fParticleID) {

            if (particle == "El") {fParticleTopologies[fParticleCount] = fElTopologies;}
            else if (particle == "P") {fParticleTopologies[fParticleCount] = fPTopologies;}
            else if (particle == "Pip") {fParticleTopologies[fParticleCount] = fPipTopologies;}
            else if (particle == "Pim") {fParticleTopologies[fParticleCount] = fPimTopologies;}
            else {
                std::cout<<"ERROR : Unknown particle type"<<std::endl;
                exit(1);
            }

            fParticleCount++;

        }
        fSplitTopologies = true;
        std::cout<<"Added topologies for " << fParticleCount << " particles"<< std::endl;
    }

    else {
        std::cout<<"ERROR: Topologies already set"<<std::endl;
        std::cout<<"       Exiting..."<<std::endl;
        exit(1);
    }
}

/**
 * Get the variables for a particular topology
 *
 */

void THSMVA::GetNamesTopo(Int_t Topology) {

    fSelectNames = {};
    fSelectedParticles = {};

    if (!fSplitTopologies) {
        std::cout<<"Topologies for particles not set..."<<std::endl;
        std::cout<<"Trying to set topologies..."<<std::endl;
        SetTopologies();
    }

    fParticleCount = 0;
    for (auto const& v : fParticleTopologies) {
        if (std::find(v.begin(), v.end(),Topology)!=v.end()) {
            fSelectedParticles.push_back(fParticleCount);
        }
        fParticleCount++;
    }
    
    std::cout<<"Number of particles in topology " <<Topology<<": "<<fSelectedParticles.size()<<std::endl; 

    for (auto const& i : fSelectedParticles) {
        fSelectNames.push_back(fNames[i]);
    }
}

/**
 * Print the variable names
 *
 */

void THSMVA::PrintVar() {
    
    // check sizes
    std::cout<<"Variable Names :"<<std::endl;
    for (auto const& names : fNames){
        for (auto const& n: names) {
            std::cout<<n<<std::endl;
        }
    }
}

/**
 * Print the topologies that each particles is present in
 *
 */

void THSMVA::PrintTopologies() {
    fCounter = 0;
    std::cout<<"Printing topologies for each particle..."<<std::endl;
    for (auto const& particle : fParticleTopologies) {
        std::cout<<"Particle: "<<fCounter<<std::endl; fCounter++;
        for (auto const& t : particle) {
            std::cout<<t<<std::endl;
        }
    }
}

/**
 * Print information about splits
 *
 */

void THSMVA::PrintSplits(){
    
    std::cout<<"Printing splits..."<<std::endl;

    for (UInt_t i=0 ;i<fSplits.size(); i++){
        std::cout<<"    Split "<< i << ":" <<std::endl;
        std::cout<<"        Name: "<< fSplits[i].GetSplitName() <<std::endl;
        std::cout<<"        Number of variables: "<<fSplits[i].GetN() <<std::endl;
        std::cout<<"        Variables: ";
        for (auto const& v : fSplits[i].GetVariableNames()){
            std::cout<<v<<" ";
        }
        std::cout<<""<<std::endl;
        std::cout<<"        Pointers set: " << fSplits[i].CheckPointers() <<std::endl;
        std::cout<<"        Variable values: ";
        for (auto const& v : fSplits[i].GetVariables()){
            std::cout<<v<<std::endl;
        }
        std::cout<<"        Tree string: " << fSplits[i].GetTreeSplit() <<std::endl;
        std::cout<<"        Number of MVA particles: " << fSplits[i].GetMVAVariables().size() <<std::endl;
        if (fPrintVariables){
            std::cout<<"        Tree variables: ";
            for (auto const& v : fSplits[i].GetMVAVariables()){
                for (auto const& p : v){
                    std::cout<<p<<" ";
                }
            }
            std::cout<<""<<std::endl;
        }
        
    }
}


//////////////////////////////
// Split class functions
//////////////////////////////

/*
 * Default constructor
 *
 */

Split::Split(){

}

/*
 * Constructor to be used when creating a split
 *
 */

Split::Split(TString inputName, std::vector<TString> inputVariableNames, std::vector<Int_t> values){
    fSplitName = inputName;
    fSplitVariableNames = inputVariableNames;
    fSplitVariables = values;
    fSplitNelements = inputVariableNames.size();
}

/*
 * Store the variables and pointers used for a split
 *
 */

void Split::AddMVAVariables(std::vector<std::vector<TString>> names, std::vector<std::vector<Float_t>> treeVars) {
    fSplitMVAVariables = names;
    fSplitMVATreeVars = treeVars;
}

/**
 * Return the string needed to clone a tree based on the split
 *
 */

TString Split::GetTreeSplit(){
    fTreeSplit = "";
    for (UInt_t i=0; i<fSplitNelements ; i++){
        fTreeSplit += "&" + fSplitVariableNames[i] + "=="  + std::to_string(fSplitVariables[i]);
    }

    return fTreeSplit;
}
