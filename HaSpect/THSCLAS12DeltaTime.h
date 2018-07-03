///////////////////////////////////////////////////
///Persistant cuts base class for use with THSFinalState
///Defaults to time cuts on each partcle
#ifndef THSCLAS12DELTATIME_h
#define THSCLAS12DELTATIME_h

#include "THSCuts.h"

class THSCLAS12DeltaTime: public THSCuts {

public :
  THSCLAS12DeltaTime()=default;
  THSCLAS12DeltaTime(TString name):THSCuts(name){};
  ~THSCLAS12DeltaTime()=default;
  
  
  Short_t Detector(Int_t det);
  Bool_t Cherenkov(Int_t det);

  Bool_t ElCut(THSParticle* part);
  Bool_t ProtCut(THSParticle* part);
  Bool_t PionPCut(THSParticle* part);
  Bool_t PionMCut(THSParticle* part);
  Bool_t KaonPCut(THSParticle* part);
  Bool_t KaonMCut(THSParticle* part);
  Bool_t GammaCut(THSParticle* part);

  void SetElCut(Float_t v0,Float_t v1){fElTimeCut[0]=v0;fElTimeCut[1]=v1;};
  void SetProtCut(Float_t v0,Float_t v1,Float_t v2,Float_t v3){fProtTimeCut[0]=v0;fProtTimeCut[1]=v1;fProtTimeCut[2]=v2;fProtTimeCut[3]=v3;};
  void SetPionPCut(Float_t v0,Float_t v1,Float_t v2,Float_t v3){fPionPTimeCut[0]=v0;fPionPTimeCut[1]=v1;fPionPTimeCut[2]=v2;fPionPTimeCut[3]=v3;};
  void SetPionMCut(Float_t v0,Float_t v1,Float_t v2,Float_t v3){fPionMTimeCut[0]=v0;fPionMTimeCut[1]=v1;fPionMTimeCut[2]=v2;fPionMTimeCut[3]=v3;};
  void SetKaonPCut(Float_t v0,Float_t v1,Float_t v2,Float_t v3){fKaonPTimeCut[0]=v0;fKaonPTimeCut[1]=v1;fKaonPTimeCut[2]=v2;fKaonPTimeCut[3]=v3;};
  void SetKaonMCut(Float_t v0,Float_t v1,Float_t v2,Float_t v3){fKaonMTimeCut[0]=v0;fKaonMTimeCut[1]=v1;fKaonMTimeCut[2]=v2;fKaonMTimeCut[3]=v3;};
  void SetGammaCut(Float_t v0,Float_t v1,Float_t v2,Float_t v3){fGammaTimeCut[0]=v0;fGammaTimeCut[1]=v1;fGammaTimeCut[2]=v2;fGammaTimeCut[3]=v3;};


  
 protected:
  Float_t fElTimeCut[2]={2,2};
  //  Float_t fProtTimeCut[4]={FT,FD,CD,FDCAL};
  Float_t fProtTimeCut[4]={0,2,2,2};
  Float_t fPionPTimeCut[4]={0,2,2,2};
  Float_t fPionMTimeCut[4]={0,2,2,2};
  Float_t fKaonPTimeCut[4]={0,2,2,2};
  Float_t fKaonMTimeCut[4]={0,2,2,2};
  Float_t fGammaTimeCut[4]={2,2,2,2};
   
};

inline Short_t THSCLAS12DeltaTime::Detector(Int_t det){
  if(det<0) return 0; //FT
  else if(det==0) return 10; //undefined
  else if(det<9999) return 1; //FD
  else if(det==10000) return 2; //CD
  else if(det%1000>99) return 3; //FDCAL no scintillator
  return 10;
}

//////////////////////////////////////////////////
///Timing cuts for CLAS12 electrons
inline Bool_t THSCLAS12DeltaTime::ElCut(THSParticle* part){
  // if(part->Time()==0) return kFALSE;
  
  Short_t det=Detector(part->Detector());
  if(det>1) //CD or unkown
    return kFALSE;
  //FT det=0, FD det=1
  if(TMath::Abs(part->DeltaTime())<fElTimeCut[det]) return kTRUE;
  
  return kFALSE;
}

//////////////////////////////////////////////////
///Timing cuts for CLAS12 protons
inline Bool_t THSCLAS12DeltaTime::ProtCut(THSParticle* part){
  if(part->Time()==0) return kFALSE;
  
  Short_t det=Detector(part->Detector());
  if(det>3) return kFALSE;
  //FT det=0, FD det=1, CD det=2, FDCAL det=3
  if(TMath::Abs(part->DeltaTime())<fProtTimeCut[det]) return kTRUE;
  
  return kFALSE;
}

//////////////////////////////////////////////////
///Timing cuts for CLAS12 pi+
inline Bool_t THSCLAS12DeltaTime::PionPCut(THSParticle* part){
  //if(part->Time()==0) return kFALSE;
  
  Short_t det=Detector(part->Detector());
  if(det>3) return kFALSE;
  //FT det=0, FD det=1, CD det=2, FDCAL det=3
  if(TMath::Abs(part->DeltaTime())<fPionPTimeCut[det]) return kTRUE;
  
  return kFALSE;
}
//////////////////////////////////////////////////
///Timing cuts for CLAS12 pi-
inline Bool_t THSCLAS12DeltaTime::PionMCut(THSParticle* part){
  // if(part->Time()==0) return kFALSE;
  
  Short_t det=Detector(part->Detector());
  if(det>3) return kFALSE;
  //FT det=0, FD det=1, CD det=2, FDCAL det=3
  if(TMath::Abs(part->DeltaTime())<fPionMTimeCut[det]) return kTRUE;
  
  return kFALSE;
}
//////////////////////////////////////////////////
///Timing cuts for CLAS12 K+
inline Bool_t THSCLAS12DeltaTime::KaonPCut(THSParticle* part){
  if(part->Time()==0) return kFALSE;
  
  Short_t det=Detector(part->Detector());
  if(det>3) return kFALSE;
  //FT det=0, FD det=1, CD det=2, FDCAL det=3
  if(TMath::Abs(part->DeltaTime())<fKaonPTimeCut[det]) return kTRUE;
  
  return kFALSE;
}
//////////////////////////////////////////////////
///Timing cuts for CLAS12 K-
inline Bool_t THSCLAS12DeltaTime::KaonMCut(THSParticle* part){
  if(part->Time()==0) return kFALSE;
  
  Short_t det=Detector(part->Detector());
  if(det>3) return kFALSE;
  //FT det=0, FD det=1, CD det=2, FDCAL det=3
  if(TMath::Abs(part->DeltaTime())<fKaonMTimeCut[det]) return kTRUE;
  
  return kFALSE;

}
//////////////////////////////////////////////////
///Timing cuts for CLAS12 Gamma
inline Bool_t THSCLAS12DeltaTime::GammaCut(THSParticle* part){
  if(part->Time()==0) return kFALSE;
  
  Short_t det=Detector(part->Detector());
  if(det>3) return kFALSE;
  //FT det=0, FD det=1, CD det=2, FDCAL det=3
  if(TMath::Abs(part->DeltaTime())<fGammaTimeCut[det]) return kTRUE;
  
  return kFALSE;
}

#endif
