#ifndef THSMVAAPP_h
#define THSMVAAPP_h

#include <TString.h>
#include <vector>

#include "THSMVATrain.h"
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#include "TMVA/MethodCuts.h"
#include "TStopwatch.h"
#include "TH2F.h"
#include "TH2D.h"
#include <THSParticle.h>


class THSMVAApp : public THSMVA {
        
    public :
        THSMVAApp();
        virtual ~THSMVAApp();
    
    protected :
        // output file
        TFile* fOutputFile=nullptr;//!
        // output trees
        TTree* fOutputTree=nullptr;//!
        TTree* fAppTree=nullptr;//!
        // vectors to loop over and fill
        std::vector<TMVA::Reader *> fReaders;
        std::vector<Float_t> fMethodResults;  
        // TMVAReader
        TMVA::Reader * fReader=nullptr;//!

        TString fOutputName;
        TString fMethodName;
        TString fWeightFile;

        TStopwatch fSW;

        Float_t fProb=0.0;

        UInt_t fSplitCount=0;
        Int_t fEventCount=0;
        Int_t fReaderIdx=0;
        Int_t fIdx=0;
        // add branches for variables used for MVA
        Bool_t fCheckVariables = kFALSE;
        // variables for output TTree
        std::vector<std::vector<Float_t>> fAppVars;
        std::vector<std::vector<TString>> fAppVariableNames;

    public:
        void SetAppTree(TTree* tree){fAppTree = tree;};
        void SetInputTree(TTree* tree){fAppTree = tree;};

        void SetOutputTree(TTree* tree){fOutputTree = tree;};
        void SetOutputFile(TFile* file){fOutputFile = file;};

        void AddVarsFromParticle(THSParticle* tmpParticle, Int_t tmpPCount);
        void SetInputBranches();
        void AddReaders();
        void SetReaderVariables(TMVA::Reader* tmpReader);
        void SetReaderVariables(TMVA::Reader* tmpReader, Split tmpSplit);
        void SetReaders();
        void SetReaderMethods(TMVA::Reader* tmpReader);
        void SetReaderMethods(TMVA::Reader* tmpReader, Split tmpSplit);
        void SetOutput();
        void ProcessTree();
        void ProcessEvent();
        Int_t CheckSplits();
        void EndApp();

        void DefaultApp();
        void Init(THSMVA * setup);
        void UpdateSplit(TString name, Int_t * pointer);
        void UpdateSplit(TString name, std::vector<Int_t *> pointers);

        Float_t GetMethodResult(Int_t i){return fMethodResults[i];}
  
};

#endif //
