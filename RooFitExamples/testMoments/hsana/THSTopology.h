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

  THSParticleIter *fIter=nullptr;
  vector<Short_t> fTrueDefinition; //pdg codes needed for this topology
  vector<Short_t> fActualDefinition; //pids needed for this topology
  vector<Short_t> fIncParts; //particle allowed to be inclusive
  vector<Short_t> fChargeParts; //particle allowed be IDed by charge

  //Flag to do PID by charge not given pdg code
  Bool_t fUseChargePID=kFALSE;
  Bool_t fIsInclusive=kFALSE;

  Int_t fID; //reference number

  const Int_t fNoID=1E4;
  
  THSTopology* fAlternative=nullptr; //Another topology with same detected final state

 public:
  Bool_t CheckTopo(vector<Short_t> *detTopo);
  Bool_t CheckExclusiveTopo(vector<Short_t> *detTopo);
  Bool_t CheckInclusiveTopo(vector<Short_t> *detTopo);
  void SetInclusive(TString parts);
  void SetChargePID(TString parts);

  void SetIter(THSParticleIter* iter){fIter=iter;};
  THSParticleIter* Iter(){return fIter;}

  void SetAlternative(THSTopology* alt){fAlternative=alt;}
  THSTopology*  Alternative(){return fAlternative;}
  vector<Short_t>* Definition() {return &fActualDefinition;}
  vector<Short_t>* True() {return &fTrueDefinition;}

  void SetID(Int_t id){fID=id;}
  Int_t ID(){return fID;}

  Short_t PDGtoCharge(Short_t pdg);
  Short_t ParticleID(Short_t pdg);
  void TopoToCharge(vector<Short_t> *thisTopo);
  
  UInt_t HowManyTrue(Short_t pdg);
  
  void Print(Int_t verbose);
};

#endif
