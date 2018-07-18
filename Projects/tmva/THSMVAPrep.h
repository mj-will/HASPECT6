#ifndef THSMVAPREP_h
#define THSMVAPREP_h

#include <TTree.h>
#include <TString.h>
#include <TFile.h>
#include <THSParticle.h>

#include "THSMVA.h"

class THSMVAPrep : public THSMVA {

    public :
        THSMVAPrep();
        virtual ~THSMVAPrep();
  
        THSMVAPrep(TString name) : THSMVA(name){};
    protected :

        TTree* fBaseTree=nullptr;//!

        Int_t fCountF = 0;
        Int_t fCountI = 0;
        UInt_t fNVarsF = 0;
        UInt_t fNVarsI = 0;

    public:

        void SetTree(TTree* tree) {fBaseTree = tree;};
        void SetBranches();
        void RemoveNaNs();
        void AddVarsFromParticle(THSParticle* tmpParticle, Int_t tmpPCount);

  ClassDef(THSMVAPrep,1) //class THSParticle

};

#endif //
