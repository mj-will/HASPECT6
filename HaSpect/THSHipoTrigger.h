//Datareader class for converting Lund format txt files
//into THSParticles
#ifndef THSHIPOTRIGGER_h
#define THSHIPOTRIGGER_h

#include "THSHipoReader.h"

class THSHipoTrigger: public THSHipoReader{

 public :
  THSHipoTrigger();
  virtual ~THSHipoTrigger(){};

  //  virtual Bool_t Init(TString filename,TString name="");
  virtual Bool_t ReadEvent(Long64_t entry=0);
  virtual void InitOutput(TString filename);

 private :

  //banks
  THipoBank* fRunConBank=nullptr;

  //items in banks
  THipoItem* fRunTrig=nullptr;
 
  
  //Trigger variables
  Int_t fFTHigh=0; 
  Int_t fFTLow=0;
  
  Int_t fTrigBits[sizeof(int) * 8];

   void CreateBitPattern(long val);
 public :
  
 
};

#endif //ifdef THSHIPOTRIGGER
