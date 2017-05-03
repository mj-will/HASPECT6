//Abstract Base class for reading different data formats and converting
//to THSParticles
#ifndef THSDATAREADER_h
#define THSDATAREADER_h

#include <TString.h>
#include <TTree.h>
#include <TFile.h>
#include <iostream>
#include <vector>
#pragma link C++ class vector<THSParticle* >+;//want to make tree branch

#include "THSParticle.h"

class THSDataReader{

 public :
  THSDataReader(){};
  ~THSDataReader();

  virtual Bool_t Init(TString filename,TString name="");
  virtual Bool_t ReadEvent(Long64_t entry=-1);
  virtual void PrintEvent(Long64_t=0);
  virtual void WriteParticles(TString filename);
  Int_t GetN(){return fNin;}
  THSParticle* GetParticle(Int_t i){return fParticles.at(i);}
  void AddParticle(THSParticle* part){fParticles.push_back(part);fNin++;};
  
  void SetReadBranch(TString name){fReadBName=name;}
  void InitOutput(TString filename);
  void WriteEvent(){fWriteTree->Fill();}
  void CloseOutput();
  
 protected :
  vector<THSParticle*> fParticles; //vector of input particles
  vector<THSParticle*> * fReadParticles; //vector of input particles from tree
  Int_t fNin; //number of iinput particles

  //default THSParticles tree reader
  TFile* fReadFile=0;
  TTree* fReadTree =0;
  Long64_t fEntry=0;
  TString fReadBName="Particles";
  //default THSParticles tree writer
  TFile* fWriteFile=0;
  TTree* fWriteTree=0;
  

 public :
 
  

};

#endif //ifdef THSDATAREADER
