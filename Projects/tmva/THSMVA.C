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

////////////////////////////////////////////////////////////
/// Here I put Doxygen readable comments explaing what each function does

THSMVA::THSMVA(){

}

/**
 * Print the variables that have been set.
 *
 */

void THSMVA::PrintVars(){
    std::cout<<"Particles : "<<std::endl;
    for (UInt_t i=0; i < particleNames.size(); i++){
        std::cout<<"        "<< particleNames[i]<<std::endl;
    }
    std::cout<<"Particle propeties : "<<std::endl;
    for (UInt_t i=0; i < particleProperties.size(); i++){
        std::cout<<"                 "<< particleProperties[i]<<std::endl;
    }
    std::cout<<"Variables : "<<std::endl;
    for (UInt_t i=0; i < variableNames.size(); i++){
        std::cout<<"        "<< variableNames[i]<<std::endl;
    }
    std::cout<<"Number of variables : "<<nVars<<std::endl;
}

        


/**
 * Checks if a tree contains any NaNs and replaces them with zeros.
 *
 */

void THSMVA::CheckTree(){
    // number of variables
    nVars = variableNames.size();
    // loop over variables and replace NaNs
    for (UInt_t i=0; i<fTrainTree->SetEntries(); i++) {
        fTrainTree->GetEntry(i);
        for (UInt_t ivar=0; ivar<nVars; ivar++) {
            if (std::isnan(treevars[ivar])) {
                vars[ivar] = 0;
            }
            else {
                vars[ivar] = treevars[ivar];
            }
        }
    }

}

/**
 * Set variable names for topology 0
 *
 */

void THSMVA::SetVariablesTopo_0(){
    // Set branches in loop
    particleNames = {"El", "P","Pip","Pim"};
    variableNames = {};

    for(auto const& pn: particleNames) {
        for(auto const& pp: particleProperties) {
            variableNames.push_back(pn+pp);
        }
    }
    nVars = variableNames.size();
}

/**
 * Set variable names for topology 1
 *
 */

void THSMVA::SetVariablesTopo_1(){
    // Set branches in loop
    particleNames = {"El", "P","Pip"};
    variableNames = {"NPerm", "NDet", "Detector"};


    for(auto const& pn: particleNames) {
        for(auto const& pp: particleProperties) {
            variableNames.push_back(pn+pp);
        }
    }
    nVars = variableNames.size();
}

/**
 * Set variable names for topology 2
 *
 */

void THSMVA::SetVariablesTopo_2(){
    // Set branches in loop
    particleNames = {"El", "P","Pim"};
    variableNames = {"NPerm", "NDet", "Detector"};


    for(auto const& pn: particleNames) {
        for(auto const& pp: particleProperties) {
            variableNames.push_back(pn+pp);
        }
    }
    nVars = variableNames.size();
}

/**
 * Set variable names for topology 3
 *
 */

void THSMVA::SetVariablesTopo_3(){
    // Set branches in loop
    particleNames = {"El","Pip","Pim"};
    variableNames = {"NPerm", "NDet", "Detector"};


    for(auto const& pn: particleNames) {
        for(auto const& pp: particleProperties) {
            variableNames.push_back(pn+pp);
        }
    }
    nVars = variableNames.size();
}
