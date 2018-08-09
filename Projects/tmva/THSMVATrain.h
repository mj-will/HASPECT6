#ifndef THSMVATRAIN_h
#define THSMVATRAIN_h

#include <TTree.h>
#include <TString.h>
#include <TFile.h>

#include "THSMVA.h"
#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
#include "TMVA/DataSetInfo.h"

class THSMVATrain : public THSMVA {

    public :
        THSMVATrain();
        virtual ~THSMVATrain();

        THSMVATrain(TString name) : THSMVA(name){};
    protected :

        THSMVA fMVA;

        TTree* fTrainTree=nullptr;//!
        TTree* fSignalTree=nullptr;//!
        TTree* fBackgroundTree=nullptr;//!

        TFile* fOutputFile=nullptr;//!

        TDirectory* fOutputDir=nullptr;

        TString fOutputName;
        TString fDatasetName;

        Bool_t fTest=false;
        Bool_t fSelectTopologies=false;
        Bool_t fSplit=false;
        Bool_t fPrintTree=false;

        TMVA::Factory* fFactory=nullptr;//!
        TMVA::DataLoader* fDataloader=nullptr;//!

        Double_t fSignalWeight = 1.0;
        Double_t fBackgroundWeight = 1.0;

        Int_t fTopo=0;
        Int_t fDetector=0; 
        Int_t fNTrain=0;
        Int_t fNTest=0;

        Split fTmpSplit;

        static std::vector<std::vector<TString>> fMVAVariables;
        static std::vector<std::vector<Float_t>> fMVATreeVars;

    public:

        void SetTrainTree(TTree* tree) {fTrainTree = tree;};

        void SetNTrain(Int_t N) {fNTrain = N;};
        void SetNTest(Int_t N) {fNTest = N;};

        void SetMVATreeVars();
        void SetMVAVariables();
        void SetMVAVariables(Int_t Topology);

        void AddSplit(TString inputName, TString inputVariable,Int_t * p, Int_t value);
        void AddSplit(TString inputName, std::vector<TString> inputVariables, std::vector<Int_t *> inputPointers, std::vector<Int_t> values);

        void SetSignalTree(TString Filter = "");
        void SetBackgroundTree(TString Filter = "");

        void SetOutputFile(TString fileName){fOutputFile = new TFile(fileName, "RECREATE");};

        void Setup(TString datasetName = "");
        void Train();
        void Test();
        void EnableTest(){fTest=true;};
        void DisableTest(){fTest=false;};
        void AddMethod(Method method);

        void EndTraining();

        void DefaultTrain();

        void WriteTHSMVA(TString name);

        ClassDef(THSMVATrain,1) //class THSParticle

};

#endif //
