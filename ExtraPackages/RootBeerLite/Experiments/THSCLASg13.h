//Datareader class for interacing to CLAS DST and BOS files with RootBeer
//into THSParticles
#ifndef THSCLASG13_h
#define THSCLASG13_h

#include "THSRootBeer.h"
#include <map>
typedef map< Int_t , TH1* > IntTH1_Map;

class THSCLASg13: public THSRootBeer{

 public :
  THSCLASg13();
  ~THSCLASg13()=default;

  virtual Bool_t Init(TString filename,TString name="");
  virtual Bool_t ReadEvent(Long64_t entry=0);
  Double_t HypothesisBeta(Double_t pp,Double_t mm){return pp/sqrt(pp*pp+mm*mm);}
  
 private :
 
  virtual void MakeDetected();
  virtual void MakeParticle(Int_t ip);
  virtual Bool_t MakeBeam(Float_t Tmid,Float_t Tcut);
  void MakeTruth();
  void MakeTruthParticle(Int_t ip);
  Int_t EPICSEvent();
  void LoadMCCor(TString corname);
  Double_t Correct_MCTrackMomentum(Int_t pdg,TLorentzVector p4);
  Double_t Correct_Momentum(int locRunNumber, int locID, int locSector, const TLorentzVector &locP4);
  Int_t fCurrentPlane=0;
  Float_t fCurrentEdge=0;
//momentum correction stuff
  IntTH1_Map fMCCor;

 public :
  
 
};

#endif //ifdef THSCLASG13
