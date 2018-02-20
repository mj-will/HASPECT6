//Datareader class for converting Lund format txt files
//into THSParticles and piping them through fastmc
#ifndef THSCLAS12FASTMC_h
#define THSCLAS12FASTMC_h

#include "THSLundReader.h"
#include "TFastMC.hxx"

class THSCLAS12FastMC: public THSLundReader{

 public :
  THSCLAS12FastMC(){};
  ~THSCLAS12FastMC(){};

  virtual Bool_t Init(TString filename,TString name="");
  virtual Bool_t ReadEvent(Long64_t entry=0);
  // virtual Bool_t ReadGenerated(Long64_t entry=0);

  void SetTorCurrent(Double_t curr){fTORUS=curr;}

 private :
   
  Double_t fTORUS=3770;
  TString fCFILE="conf5.dat";
 public :
  
 
};

#endif //ifdef THSCLAS12FASTMC
