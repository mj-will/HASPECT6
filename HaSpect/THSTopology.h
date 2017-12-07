#ifndef THSTOPOLOGY_h
#define THSTOPOLOGY_h

//#include <TString.h>
//#include <TNamed.h>
//#include <TTree.h>
#include <vector>
#include <functional>
//#include <map>

#include "THSParticle.h"
#include "THSParticleIter.h"
#include "THSFinalState.h"
//#include "THSDataManager.h"
//#include "THSKinematics.h"
/* #include <TTreeReader.h> */
/* #include <TTreeReaderValue.h> */
/* #include <TTreeReaderArray.h> */




class THSTopology{

 public :

  THSTopology(){};
  THSTopology(TString topo,FinalState::VoidFuncs funcE,TString chPID="",TString incl="");
  virtual ~THSTopology();

  
  FinalState::VoidFuncs Exec;
 private:

  vector<Int_t> fDefinition; //particle codes needed for this topology
  THSParticleIter *fIter=nullptr;
  vector<Int_t> fIncParts; //particle allowed to be inclusive
  vector<Int_t> fChargeParts; //particle allowed be IDed by charge

  //Flag to do PID by charge not given pdg code
  Bool_t fUseChargePID=kFALSE;
  Bool_t fIsInclusive=kFALSE;

  Int_t fID; //reference number

  const Int_t fNoID=1E6;
  
  THSTopology* fAlternative=nullptr; //Another topology with same detected final state

 public:
  Bool_t CheckTopo(vector<Int_t> *detTopo);
  Bool_t CheckExclusiveTopo(vector<Int_t> *detTopo);
  Bool_t CheckInclusiveTopo(vector<Int_t> *detTopo);
  void SetInclusive(TString parts);
  void SetChargePID(TString parts);

  void SetIter(THSParticleIter* iter){fIter=iter;};
  THSParticleIter* Iter(){return fIter;}

  void SetAlternative(THSTopology* alt){fAlternative=alt;}
  THSTopology*  Alternative(){return fAlternative;}
  vector<Int_t>* Definition() {return &fDefinition;}

  void SetID(Int_t id){fID=id;}
  Int_t ID(){return fID;}

  Int_t PDGtoCharge(Int_t pdg);
  Int_t ParticleID(Int_t pdg);
  void TopoToCharge(vector<Int_t> *thisTopo);
  
  void Print();
};

#endif
