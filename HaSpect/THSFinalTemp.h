
#ifndef THSFINALTEMP_h
#define THSFINALTEMP_h

#include "THSFinalState.h"
#include "THSParticle.h"
#include <vector>

class THSFinalTemp : public THSFinalState{

 public :
  THSFinalTemp();
  ~THSFinalTemp()=default;


  //Init functions
  void Init_Generated();
  //void Init_IterX();
  //void Topo_X();

  void Kinematics();
  protected :
 
  //Initial state
  HSLorentzVector fBeam=HSLorentzVector(0,0,10.6,10.6);
  HSLorentzVector fTarget=HSLorentzVector(0,0,0,0.938272);

  //Final Particles Detected
 
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

   public :
  virtual void FinalStateOutTree(TTree* tree);

 

};

#endif //ifdef THSFinalTemp
