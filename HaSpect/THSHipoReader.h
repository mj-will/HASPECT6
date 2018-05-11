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
  // virtual void WriteParticles(TString filename);
  virtual void InitOutput(TString filename);
  virtual void CloseOutput();

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
  THipoBank *fFTCALClustBank=nullptr;
   //fHipo->GetBank("FTCAL::clusters");

  THipoItemI* fPid=nullptr;
  THipoItemF* fPx=nullptr;
  THipoItemF* fPy=nullptr;
  THipoItemF* fPz=nullptr;
  THipoItemF* fVx=nullptr;
  THipoItemF* fVy=nullptr;
  THipoItemF* fVz=nullptr;
  THipoItemF* fBeta=nullptr;
  THipoItemB* fCharge=nullptr;
  THipoItemS* fStatus=nullptr;
  
  THipoItemS* fSPindex=nullptr;
  THipoItemF* fSTime=nullptr;
  THipoItemB* fSSector=nullptr;
  THipoItemF* fSEnergy=nullptr;
  THipoItemB* fSDet=nullptr;
  THipoItemF* fSPath=nullptr;

  THipoItemS* fCalPindex=nullptr;
  THipoItemF* fCalEnergy=nullptr;
  THipoItemF* fCalTime=nullptr;
  THipoItemF* fCalPath=nullptr;

  THipoItemS* fChPindex=nullptr;
  THipoItemS* fChEnergy=nullptr;
  THipoItemB* fChDetector=nullptr;

  THipoItemS* fFTPindex=nullptr;
  THipoItemF* fFTTime=nullptr;
  THipoItemF* fFTEnergy=nullptr;
  THipoItemB* fFTDet=nullptr;
  THipoItemF* fFTPath=nullptr;
  THipoItemF* fFTX=nullptr;
  THipoItemF* fFTY=nullptr;
  THipoItemF* fFTZ=nullptr;

  THipoItemF* fRecEvSTTime=nullptr;
 
  THipoItemI* fMCPid=nullptr;
  THipoItemF* fMCPx=nullptr;
  THipoItemF* fMCPy=nullptr;
  THipoItemF* fMCPz=nullptr;
  THipoItemF* fMCVx=nullptr;
  THipoItemF* fMCVy=nullptr;
  THipoItemF* fMCVz=nullptr;
  
  THipoItemF* fFTCALClust_t=nullptr;

  Bool_t fUsePID=kTRUE;
  Bool_t fCombineFiles=kFALSE;
  Int_t fRunNumber=0;
 public :
  
 
};

#endif //ifdef THSLUNDREADER
