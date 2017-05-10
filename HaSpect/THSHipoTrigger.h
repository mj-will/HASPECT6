//Datareader class for converting Lund format txt files
//into THSParticles
#ifndef THSHIPOTRIGGER_h
#define THSHIPOTRIGGER_h

#include <iostream>
#include <fstream>
#include "THSHipoReader.h"

class THSHipoTrigger: public THSHipoReader{

 public :
  THSHipoTrigger();
  ~THSHipoTrigger(){};

  virtual Bool_t Init(TString filename,TString name="");
  virtual Bool_t ReadEvent(Long64_t entry=0);
  virtual void InitOutput(TString filename);

 private :

  //banks
  THipoBank* fFTCalBank=nullptr;

  //items in banks
  THipoItem* fFTCal_E=nullptr;
  
  //Trigger variables
  Float_t fFTEsum=0;
  Bool_t fPassTrig1=kFALSE;
  
 public :
  
 
};

#endif //ifdef THSHIPOTRIGGER
