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
  virtual ~THSHipoReader(){if(fHipo) delete fHipo;};

  virtual Bool_t Init(TString filename,TString name="");
  virtual Bool_t ReadEvent(Long64_t entry=0);
  virtual void CloseReadTree();
  virtual void WriteParticles(TString filename);
  // void SetUsePID(Bool_t use=kTRUE){fUsePID=use;}
  
  Int_t GetRunNumber(TString filen);
  void SetCombineFiles(Bool_t setf=kTRUE){fCombineFiles=kTRUE;};
 protected :
  
  THipo *fHipo=nullptr;
  THipoBank *fPBank=nullptr;
  THipoBank *fSBank=nullptr;
  THipoBank *fCalBank=nullptr;
  THipoBank *fChBank=nullptr;
  THipoBank *fMCBank=nullptr;
  THipoBank *fFTBank=nullptr;
  THipoBank *fEvBank=nullptr;
  //  THipoBank *fCVTBank=nullptr;

  THipoItem* fPid=nullptr;
  THipoItem* fPx=nullptr;
  THipoItem* fPy=nullptr;
  THipoItem* fPz=nullptr;
  THipoItem* fVx=nullptr;
  THipoItem* fVy=nullptr;
  THipoItem* fVz=nullptr;
  THipoItem* fBeta=nullptr;
  THipoItem* fCharge=nullptr;
  
  THipoItem* fSPindex=nullptr;
  THipoItem* fSTime=nullptr;
  THipoItem* fSSector=nullptr;
  THipoItem* fSEnergy=nullptr;
  THipoItem* fSDet=nullptr;
  THipoItem* fSPath=nullptr;

  THipoItem* fCalPindex=nullptr;
  THipoItem* fCalEnergy=nullptr;
  THipoItem* fCalTime=nullptr;
  THipoItem* fCalPath=nullptr;

  THipoItem* fChPindex=nullptr;
  THipoItem* fChEnergy=nullptr;
  THipoItem* fChDetector=nullptr;

  THipoItem* fFTPindex=nullptr;
  THipoItem* fFTTime=nullptr;
  THipoItem* fFTEnergy=nullptr;
  THipoItem* fFTDet=nullptr;
  THipoItem* fFTPath=nullptr;

  THipoItem* fRecEvSTTime=nullptr;
 
  THipoItem* fMCPid=nullptr;
  THipoItem* fMCPx=nullptr;
  THipoItem* fMCPy=nullptr;
  THipoItem* fMCPz=nullptr;
  THipoItem* fMCVx=nullptr;
  THipoItem* fMCVy=nullptr;
  THipoItem* fMCVz=nullptr;

  Bool_t fUsePID=kTRUE;
  Bool_t fCombineFiles=kFALSE;
  Int_t fRunNumber=0;
 public :
  
 
};

#endif //ifdef THSLUNDREADER
