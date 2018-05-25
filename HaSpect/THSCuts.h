///////////////////////////////////////////////////
///Persistant cuts base class for use with THSFinalState
///Defaults to time cuts on each partcle
#ifndef THSCUTS_h
#define THSCUTS_h

#include <TNamed.h>
#include <TMath.h>
#include "THSParticle.h"

class THSCuts: public TNamed {
 public :
  THSCuts()=default;
 THSCuts(TString name):TNamed(name,name){};
  ~THSCuts()=default;
  
  
  virtual Bool_t ParticleCut(THSParticle* part);
  void SetTimeCut(Float_t val){fTimeCut=val;}
  
 protected:
  Float_t fTimeCut=0;
  
};

inline Bool_t THSCuts::ParticleCut(THSParticle* part){
  if(TMath::Abs(part->DeltaTime())<fTimeCut) return kTRUE;
  return kFALSE;
}

#endif
