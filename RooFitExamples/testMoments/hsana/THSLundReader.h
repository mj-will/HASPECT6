//Datareader class for converting Lund format txt files
//into THSParticles
#ifndef THSLUNDREADER_h
#define THSLUNDREADER_h

#include <iostream>
#include <fstream>

#include "THSParticle.h"
#include "THSDataManager.h"

class THSLundReader: public THSDataManager{

 public :
  THSLundReader(){};
  ~THSLundReader(){fTxtIn.close();};

  virtual Bool_t Init(TString filename,TString name="");
  virtual Bool_t ReadEvent(Long64_t entry=0);
  virtual Bool_t ReadGenerated(Long64_t entry=0);


 private :
  
  ifstream fTxtIn; //input text file stream
  
 public :
  
 
};

#endif //ifdef THSLUNDREADER
