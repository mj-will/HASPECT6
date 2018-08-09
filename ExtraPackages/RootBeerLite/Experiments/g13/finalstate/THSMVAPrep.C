/**
	\class THSMVAPrep
	For creating TMVA training tree in THSFinalState
	Given a THSParticle it will create default branches
	from datamembers of the THSParticle class
	For each combitorial event it will then fill the tree

*/
#include "THSMVAPrep.h"
#include "TROOT.h"


ClassImp(THSMVAPrep);


////////////////////////////////////////////////////////////
/// Here I put Doxygen readable comments explaing what each function does
/// Constructor 

THSMVAPrep::THSMVAPrep(){

}

/*
 * Set branches for variables necessary for MVA
 *
 */

void THSMVAPrep::SetBranches() {
    if (!fBaseTree) {
        std::cout<<"ERROR : Base tree not found..."<<std::endl;
        std::cout<<"      : exiting..."                <<std::endl;
        exit(1);
    }

    std::cout<<"Setting branches..."<<std::endl;
    
    fVariableCount = 0;
    fParticleCount = 0;

    // Now add branches for particles and variables
    std::cout<<"Printing variables: "<<std::endl;
    for (auto const& p : fNames ) { 
        for (auto const& n : p) {
            std::cout<<"            "<<n<<"   ";
            if (fTypes[fVariableCount] == "F") {
                std::cout<<fParticleCount << " / " <<fVariableCount<<std::endl;
                fBaseTree->Branch(n, &(fTreeVarsF[fParticleCount][fVariableCount]), n  + "/F");
            }
            if (fTypes[fVariableCount] == "I") {
                std::cout<<fParticleCount << " / " <<fVariableCount<<std::endl;
                fBaseTree->Branch(n, &(fTreeVarsI[fParticleCount][fVariableCount]), n  + "/I");
            }
            fVariableCount++;
        }
        fVariableCount = 0;
        fParticleCount++;
        
    }
    fNVarsF = fTreeVarsF[0].size();
    fNVarsI = fTreeVarsI[0].size();
    std::cout<<"Number of variables per particle (Float_t) :    "<<fNVarsF<<std::endl;
    std::cout<<"Number of variables per particle (Int_t)   :    "<<fNVarsI<<std::endl;

}

/**
 * Remove NaN values from an entry
 *
 */

void THSMVAPrep::RemoveNaNs(){

    //std::cout<<"Removing NaNs..."<<std::endl;

    //if (!fBaseTree) {
    //    std::cout<<"ERROR: Base tree not set..."<<std::endl;
    //    std::cout<<"Exiting.."<<std::endl;
    //    exit(1);
    //}

    for (UInt_t iPar=0; iPar<fParticleID.size(); iPar++) {
        //std::cout<<"...floats..."<<std::endl;
        for (UInt_t iVar=0; iVar<fNVarsF; iVar++) {
            if (std::isnan(fTreeVarsF[iPar][iVar])){
                fTreeVarsF[iPar][iVar] = 0;
            }
        }
        //std::cout<<"...ints..."<<std::endl;
        for (UInt_t iVar=0; iVar<fNVarsI; iVar++) {
            if (std::isnan(fTreeVarsI[iPar][iVar])){
                fTreeVarsI[iPar][iVar] = 0;
            }
        }
    } 

    //fBaseTree->Fill();
    
}

/**
 * Add variables from a THSParticles with predefined options
 *
 */

void THSMVAPrep::AddVarsFromParticle(THSParticle* tmpParticle, Int_t tmpPCount) {
    fCountF = 0;
    fCountI = 0;

    if (!fVarNames.empty()){
        for (auto const& v : fVarNames[tmpPCount]) {
            if (v == "Time") {fTreeVarsF[tmpPCount][fCountF] = tmpParticle->DeltaTime(); fCountF++;};
            if (v == "Edep") {fTreeVarsF[tmpPCount][fCountF] = tmpParticle->Edep(); fCountF++;};
            if (v == "DeltaE") {fTreeVarsF[tmpPCount][fCountF] = tmpParticle->DeltaE(); fCountF++;};
            if (v == "PreE") {fTreeVarsF[tmpPCount][fCountF] = tmpParticle->PreE(); fCountF++;};
            if (v == "P") {fTreeVarsF[tmpPCount][fCountF] = tmpParticle->P4p()->P(); fCountF++;};
            if (v == "Th") {fTreeVarsF[tmpPCount][fCountF] = tmpParticle->P4p()->Theta(); fCountF++;};
            if (v == "Phi") {fTreeVarsF[tmpPCount][fCountF] = tmpParticle->P4p()->Phi(); fCountF++;};
            if (v == "Vz") {fTreeVarsF[tmpPCount][fCountF] = tmpParticle->Vertex().Z(); fCountF++;};
            if (v == "TrChi2") {fTreeVarsF[tmpPCount][fCountF] = tmpParticle->TrChi2(); fCountF++;};
            // convert to float for training
            if (v == "Det") {fTreeVarsI[tmpPCount][fCountI] = tmpParticle->Detector(); fCountI++;
            }
        }
    }
    else{
        for (auto const& v : fVariableID) {
            if (v == "Time") {fTreeVarsF[tmpPCount][fCountF] = tmpParticle->DeltaTime(); fCountF++;};
            if (v == "Edep") {fTreeVarsF[tmpPCount][fCountF] = tmpParticle->Edep(); fCountF++;};
            if (v == "DeltaE") {fTreeVarsF[tmpPCount][fCountF] = tmpParticle->DeltaE(); fCountF++;};
            if (v == "PreE") {fTreeVarsF[tmpPCount][fCountF] = tmpParticle->PreE(); fCountF++;};
            if (v == "P") {fTreeVarsF[tmpPCount][fCountF] = tmpParticle->P4p()->P(); fCountF++;};
            if (v == "Th") {fTreeVarsF[tmpPCount][fCountF] = tmpParticle->P4p()->Theta(); fCountF++;};
            if (v == "Phi") {fTreeVarsF[tmpPCount][fCountF] = tmpParticle->P4p()->Phi(); fCountF++;};
            if (v == "Vz") {fTreeVarsF[tmpPCount][fCountF] = tmpParticle->Vertex().Z(); fCountF++;};
            if (v == "TrChi2") {fTreeVarsF[tmpPCount][fCountF] = tmpParticle->TrChi2(); fCountF++;};
            // convert to float for training
            if (v == "Det") {fTreeVarsI[tmpPCount][fCountI] = tmpParticle->Detector(); fCountI++;
            }
        }
    }
}

///////////////////////////////////////////////////////////
///Destructor, here I need to delete any data members that have
///been constructed via the new operator and not been deleted elsewhere
THSMVAPrep::~THSMVAPrep(){
    delete fBaseTree;
}
