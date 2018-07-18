
#ifndef THS2PI_h
#define THS2PI_h

#include "THSFinalState.h"
#include "THSParticle.h"
#include "THSCLAS12Trigger.h"
#include "THSCLAS12DeltaTime.h"
#include "THSMVAPrep.h"
#include "THSMVATrain.h"
#include <vector>

class THS2pi : public THSFinalState{

 public :
  THS2pi();
  ~THS2pi()=default;


  //Init functions
  void Init_Iter0();
  void Init_Iter1();
  void Init_Iter2();
  void Init_Iter3();
   //Topology action functions
  void Topo_0();
  void Topo_1();
  void Topo_2();
  void Topo_3();
  void Init_Generated();
  //void Init_IterX();
  //void Topo_X();
  virtual void FileStart();
  virtual Bool_t  CheckParticle(THSParticle* part);

  void PrepAddParticle(THSParticle* part);
  void FillVars();

  void Kinematics();
  protected :

  THSCLAS12Trigger fTrigger;//For CLAS12 trigger info
  THSCLAS12DeltaTime fCuts; //For particle cuts

  THSMVAPrep fMVAPrep;
  THSMVATrain fMVATrain;
  
  //Initial state
  HSLorentzVector fBeam=HSLorentzVector(0,0,10.6,10.6);
  HSLorentzVector fTarget=HSLorentzVector(0,0,0,0.938272);

  //Final Particles Detected
  THSParticle fElectron=THSParticle("e-");
  THSParticle fProton=THSParticle("proton");
  THSParticle fPip=THSParticle("pi+");
  THSParticle fPim=THSParticle("pi-");
 
  //Final Parents
 
  //Observables
  Double_t f_t;
  Double_t f_Q2;
  Double_t f_W;
  Double_t f_EGamma;
  Double_t f_Pol;
  Double_t fCMCosTh;
  Double_t fCMPhi;

  //Discriminators
  Double_t fMissMass=0;
  Double_t fMissMass2=0;

  // vector for MVA
  vector<THSParticle * > fParticles;


  //TMVA
  Bool_t fIsTMVA=kTRUE;
 public:
  virtual void TMVAOutTree(TTree* tree);
  void TMVAFill();
  void RunTraining() {fMVATrain.DefaultTrain();};
 protected:
  
  Float_t fElTime=0;
  Float_t fElEdep=0;
  Float_t fElDeltaE=0;
  Float_t fElPreE=0;
  Float_t fElP=0;
  Float_t fElTh=0;
  Float_t fElPhi=0;
  Float_t fElVz=0;
  Float_t fElTrChi2=0;
  Int_t fElDet=0;

  Float_t fPTime=0;
  Float_t fPEdep=0;
  Float_t fPDeltaE=0;
  Float_t fPPreE=0;
  Float_t fPP=0;
  Float_t fPTh=0;
  Float_t fPPhi=0;
  Float_t fPVz=0;
  Float_t fPTrChi2=0;
  Int_t fPDet=0;
  
  Float_t fPipTime=0;
  Float_t fPipEdep=0;
  Float_t fPipDeltaE=0;
  Float_t fPipPreE=0;
  Float_t fPipP=0;
  Float_t fPipTh=0;
  Float_t fPipPhi=0;
  Float_t fPipVz=0;
  Float_t fPipTrChi2=0;
  Int_t fPipDet=0;

  Float_t fPimTime=0;
  Float_t fPimEdep=0;
  Float_t fPimDeltaE=0;
  Float_t fPimPreE=0;
  Float_t fPimP=0;
  Float_t fPimTh=0;
  Float_t fPimPhi=0;
  Float_t fPimVz=0;
  Float_t fPimTrChi2=0;
  Int_t fPimDet=0;

 public :
  virtual void FinalStateOutTree(TTree* tree);

 

};

#endif //ifdef THS2pi
