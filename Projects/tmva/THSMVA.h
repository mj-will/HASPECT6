#ifndef THSMVA_h
#define THSMVA_h

#include <iostream>
#include <TNamed.h>
#include <TString.h>
#include <TTree.h>
#include <vector>

using namespace std;

// TODO: consider making class for variable, parameter and properties

class THSMVA : public TNamed {

    public :
        THSMVA();
        ~THSMVA()=default;
        THSMVA(TString name) : TNamed(name,name){};
  
    protected :

        Int_t fParticleCount = 0;
        Int_t fVariableCount = 0;
        Int_t fCounter = 0;

        Bool_t fSplitTopologies = false;

        std::vector<TString> fVariableID = {"Time", "Edep","DeltaE", "PreE", "P", "Th", "Phi", "Vz", "TrChi2", "Det"};
        std::vector<TString> fTypes = {"F", "F", "F", "F", "F", "F", "F", "F", "F", "I"};
        std::vector<TString> fParticleID = {"El", "P", "Pip", "Pim"};
        
        std::vector<Int_t> fElTopologies = {0, 1, 2, 3};
        std::vector<Int_t> fPTopologies = {0, 1, 2};
        std::vector<Int_t> fPipTopologies = {0, 1, 3};
        std::vector<Int_t> fPimTopologies = {0, 2, 3};
        std::vector<Int_t> fSelectedParticles;

        std::vector<std::vector<TString>> fNames;
        std::vector<std::vector<TString>> fSelectNames;
        std::vector<std::vector<Float_t>> fTreeVarsF;
        std::vector<std::vector<Int_t>> fTreeVarsI;
        std::vector<std::vector<Int_t>> fTopologies;
        std::vector<std::vector<Int_t>> fParticleTopologies;

    public:
        ClassDef(THSMVA,1) //class THSParticle
    
        void SetVariables(std::vector<TString> tmpVariables) {fVariableID = tmpVariables;};
        void SetTypes(std::vector<TString> tmpTypes) {fTypes = tmpTypes;};
        void SetParticles(std::vector<TString> tmpParticles) {fParticleID = tmpParticles;};
        void SetNames();
        void SetTopologies();
        void GetNamesTopo(Int_t Topology);
        void GetNames();
        void PrintVar();
        void PrintTopologies();

};

#endif //
