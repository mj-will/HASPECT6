#ifndef THSMVAPREP_h
#define THSMVAPREP_h

#include <TTree.h>
#include <TString.h>
#include <TFile.h>

#include "THSMVA.h"
#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Tools.h"
//#include <TMVA/MethodRXGB.h>

class THSMVAPrep : public THSMVA {

 public :
  THSMVAPrep();
  virtual ~THSMVAPrep();
  
 protected :

  TTree* fSignalTree=nullptr;//!
  TTree* fBackgroundTree=nullptr;//!
  TString outputName;
  TFile* outputFile=nullptr;
  Bool_t test=false;
  TMVA::Factory* factory=nullptr;
  TMVA::DataLoader* dataloader=nullptr;

  Double_t SignalWeight = 1.0;
  Double_t BackgroundWeight = 1.0;

  Int_t PrintMax = 100;
  
 public:

  void SetTree(TTree* tree){fTrainTree=tree;};
  void SetSignal();
  void SetBackground();
  void Train();
  void Test();
  void Setup();
  void EndPrep();
};

#endif //
