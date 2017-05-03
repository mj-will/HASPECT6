//Datareader class for converting Lund format txt files
//into THSParticles
#ifndef THSHIPOREADER_h
#define THSHIPOREADER_h

#include <iostream>
#include <fstream>

#include "THSParticle.h"
#include "THSDataManager.h"
#include "THipo.h"

class THSHipoReader: public THSDataManager{

 public :
  THSHipoReader();
  ~THSHipoReader(){if(fHipo) delete fHipo;};

  virtual Bool_t Init(TString filename,TString name="");
  virtual Bool_t ReadEvent(Long64_t entry=0);


 private :
  
  THipo *fHipo=nullptr;
  THipoBank *fPBank=nullptr;
  THipoBank *fMCBank=nullptr;
  THipoBank *fFTBank=nullptr;

  THipoItem* fPid=nullptr;
  THipoItem* fPx=nullptr;
  THipoItem* fPy=nullptr;
  THipoItem* fPz=nullptr;
  THipoItem* fVx=nullptr;
  THipoItem* fVy=nullptr;
  THipoItem* fVz=nullptr;
  THipoItem* fMass=nullptr;
  THipoItem* fCharge=nullptr;
  
  THipoItem* fFTPid=nullptr;
  THipoItem* fFTTime=nullptr;
  THipoItem* fFTPx=nullptr;
  THipoItem* fFTPy=nullptr;
  THipoItem* fFTPz=nullptr;
  THipoItem* fFTCharge=nullptr;
  THipoItem* fFTEnergy=nullptr;
  
  THipoItem* fMCPid=nullptr;
  THipoItem* fMCPx=nullptr;
  THipoItem* fMCPy=nullptr;
  THipoItem* fMCPz=nullptr;
  THipoItem* fMCVx=nullptr;
  THipoItem* fMCVy=nullptr;
  THipoItem* fMCVz=nullptr;
  
  
 public :
  
 
};

#endif //ifdef THSLUNDREADER
