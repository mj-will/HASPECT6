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
#include "TMVA/Types.h"
//#include <TMVA/MethodRXGB.h>

class Method {

    public :

        TMVA::Types::EMVA type;
        TString name;
        TString parameters;
};

class THSMVATrain : public THSMVA {

 public :
  THSMVATrain();
  virtual ~THSMVATrain();
  
  THSMVATrain(TString name) : THSMVA(name){};
 protected :

  TTree* fTrainTree=nullptr;//!
  TTree* fSignalTree=nullptr;//!
  TTree* fBackgroundTree=nullptr;//!

  TFile* fTreeOutputFile=nullptr;//!
  TFile* fOutputFile=nullptr;//!

  TString fTreeOutputName;
  TString fOutputName;

  Bool_t fTest=false;
  Bool_t fSelectTopologies=false;
  
  TMVA::Factory* fFactory=nullptr;//!
  TMVA::DataLoader* fDataloader=nullptr;//!

  Double_t fSignalWeight = 1.0;
  Double_t fBackgroundWeight = 1.0;

  std::vector<Method> fMethods;

  std::vector<std::vector<TString>> fMVAVariables;
  std::vector<std::vector<Float_t>> fMVATreeVars;

 public:

  void SetTrainTree(TTree* tree) {fTrainTree = tree;};

  void SetMVATreeVars();
  void SetMVAVariables();
  void SetMVAVariables(Int_t Topology);

  void SetSignalTree(TString Filter = "");
  void SetBackgroundTree(TString Filter = "");

  void Setup();
  void Train();
  void Test();
  void EnableTest(){fTest=true;};
  void DisableTest(){fTest=false;};
  void AddMethod(Method method);

  void EndTraining();

  void DefaultTrain();

  ClassDef(THSMVATrain,1) //class THSParticle

};

#endif //
