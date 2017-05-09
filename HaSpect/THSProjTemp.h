//Sample project class for omega photoproduction
#ifndef THSPROJTEMP_h
#define THSPROJTEMP_h

#include "THSProject.h"
#include "TLorentzVector.h"
#include <vector>

class THSProjTemp : public THSProject{

 public :
  THSProjTemp();
  ~THSProjTemp(){};

  virtual Bool_t WorkOnEvent();

  //Init functions
  void Init_Generated();
  //void Init_();

  void Kinematics();
  protected :
  //Topology flags for this project
  // Int_t fTID_=-1;
 
  //Initial state
  TLorentzVector fBeam=TLorentzVector(0,0,11,11.000510999);
  TLorentzVector fTarget=TLorentzVector(0,0,0,0.938272);
  //Final Particles
 
  //Observables
  Double_t f_t;
  Double_t f_Q2;
  Double_t f_W;
  Double_t f_Pol;
  Double_t fCMCosTh;
  Double_t fCMPhi;

  //Discriminators
  Double_t fMissMass=0;

   public :
  virtual void ProjectOutTree(TTree* tree);

  // ClassDef(THSProjTemp, 1)  // Writeable  class


};

#endif //ifdef THSProjTemp
