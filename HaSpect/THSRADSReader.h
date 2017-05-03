//Datareader class for converting Lund format txt files
//into THSParticles
#ifndef THSRADSREADER_h
#define THSRADSREADER_h

#include <iostream>
#include <fstream>

#include "THSParticle.h"
#include "THSDataManager.h"

class THSRADSReader: public THSDataManager{

 public :
  THSRADSReader(){};
  ~THSRADSReader(){fTxtIn.close();};

  virtual Bool_t Init(TString filename,TString name="");
  virtual Bool_t ReadEvent(Long64_t entry=0);


 private :
  
  ifstream fTxtIn; //input text file stream
  
 public :
  
 
};

#endif //ifdef THSLUNDREADER
