
#ifndef THSTEST_h
#define THSTEST_h

#include "THSFinalState.h"
#include "THSParticle.h"
#include <vector>

class THSTest : public THSFinalState{

 public :
  THSTest();
  ~THSTest()=default;


  //Init functions
  void Init_Iter0();
   //Topology action functions
  void Topo_0();
  void Init_Generated();
  //void Init_IterX();
  //void Topo_X();
  virtual void FileStart();
  virtual Bool_t  CheckParticle(THSParticle* part);

  void Kinematics();
  protected :

  
  //Initial state
  HSLorentzVector fTarget=HSLorentzVector(0,0,0,0.938272);

  //Final Particles Detected
  THSParticle fBeam=THSParticle(-22);
  THSParticle fNeutron=THSParticle("gamma");
  THSParticle fGamma1=THSParticle("gamma");
  THSParticle fGamma2=THSParticle("gamma");
 
  //Final Parents
  THSParticle fPi0=THSParticle("pi0");
 
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

#endif //ifdef THSTest
