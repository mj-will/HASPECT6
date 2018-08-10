
#ifndef THSK0L_h
#define THSK0L_h

#include "THSFinalState.h"
#include "THSParticle.h"
#include "THSCLASg13Trigger.h"

#include <vector>

class THSK0L : public THSFinalState{

 public :
  THSK0L();
  ~THSK0L()=default;


  //Init functions
  void Init_Iter0();
  void Init_Iter1();
   //Topology action functions
  void Topo_0();
  void Topo_1();
  void Init_Generated();
  //void Init_IterX();
  //void Topo_X();
  virtual void FileStart();
  virtual Bool_t  CheckParticle(THSParticle* part);

  void Kinematics();
  protected :

THSCLASg13Trigger fTrigger;	 
  //Initial state
 HSLorentzVector fTarget=HSLorentzVector(0,0,0,1.8756);
 
  //Final Particles Detected
  THSParticle fBeam=THSParticle(-22);
  THSParticle fProton=THSParticle("proton");
  THSParticle fPip=THSParticle("pi+");
  THSParticle fPimL=THSParticle("pi-");
  THSParticle fPimK=THSParticle("pi-");
  THSParticle fSpectator=THSParticle("proton");
 
  //Final Parents
  THSParticle fLambda=THSParticle("Lambda0");
  THSParticle fK0=THSParticle("K0");
  THSParticle fReaction=THSParticle();

  //Observables
  Double_t f_t;
  Double_t f_Q2;
  Double_t f_W;
  Double_t f_Pol;
  Double_t fCMCosTh;
  Double_t fCMPhi;
  Double_t fK0Phi;
  Double_t fPolCosTh;
  Double_t fPolPhi;

  Double_t fCosx;
  Double_t fCosy;
  Double_t fCosz;
  Double_t fEgamma;
  Double_t fSpecP;
  Double_t fSpecTh;
  Double_t fPol;
  Int_t fPolState;
  Int_t fFidCut;


  //generated 
  Double_t genfCMCosTh;
  Double_t genfCMPhi;
  Double_t genfCosx;
  Double_t genfCosy;
  Double_t genfCosz;
  Double_t genfPolCosTh;
  Double_t genfPolPhi;

  //Discriminators
  Double_t fMK0=0;
  Double_t fML=0;
  Double_t fMissMass=0; 
  Double_t fMissMassFix=0; 
  Double_t fMeasMass=0;
  Double_t fDeltaTime=0;
  Double_t fDeltaTimeR=0;
  Double_t fPiTime=0;
  Double_t fPTime=0;
  Double_t fPPiTime=0;
  Double_t fPPTime=0;
  Double_t fPipPiTime=0;
  Double_t fPipPTime=0;
  Double_t fMomentum=0;
  Double_t fVertex=0;
  Double_t fLVertex=0;
  Double_t fKVertex=0;
  Double_t fPiTheta=0;

   public :
  virtual void FinalStateOutTree(TTree* tree);

 

};

#endif //ifdef THSK0L
