//Datareader class for converting Lund format txt files
//into THSParticles
#ifndef THSTXTTREEREADER_h
#define THSTXTTREEREADER_h

#include <iostream>
#include <fstream>
#include <string>

#include "THSParticle.h"
#include "THSDataManager.h"

class THSTxtTreeReader: public THSDataManager{

 public :
  THSTxtTreeReader(){};
  ~THSTxtTreeReader(){fTxtIn.close();};

  virtual Bool_t Init(TString filename,TString name="");
  virtual Bool_t ReadEvent(Long64_t entry=0);


 private :
  void ReadHSParticle(THSParticle* particle);
  Double_t ReadDouble();
  Float_t ReadFloat();
  Int_t ReadInt();
  Long64_t ReadLong();

  
  ifstream fTxtIn; //input text file stream
  stringstream fStreamline; //line stream
  vector<TString> fBNames; //names of tree branches
  Int_t fNbranches=0;

 public :
  
 
};

#endif //ifdef THSTXTTREEREADER
