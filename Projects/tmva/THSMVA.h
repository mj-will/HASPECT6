#ifndef THSMVA_h
#define THSMVA_h

#include <iostream>
#include <TObject.h>
#include <TString.h>
#include <TTree.h>
#include <vector>

using namespace std;

// TODO: consider making class for variable, parameter and properties

class THSMVA : public TObject {

 public :
  THSMVA();
  ~THSMVA()=default;
  
  void PrintVars();
  void CheckTree();
  void SetVariablesTopo_0();
  void SetVariablesTopo_1();
  void SetVariablesTopo_2();
  void SetVariablesTopo_3();
  void SetTree(TTree* tree){fTrainTree = tree;};

 protected :

  TTree* fTrainTree=nullptr;//!

  // list of "properties"
  std::vector<TString> particleProperties = {"Time","Edep","DeltaE", "PreE", "P", "Th", "Phi", "Vz", "TrChi2"};
  std::vector<TString> particleNames;
  std::vector<TString> variableNames;

  Int_t nVars;
  std::vector<Double_t> vars;
  Float_t treevars[100];

 public:
  
};

#endif //
