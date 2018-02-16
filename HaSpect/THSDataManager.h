//Abstract Base class for reading different data formats and converting
//to THSParticles
#ifndef THSDATAMANAGER_h
#define THSDATAMANAGER_h

#include <TString.h>
#include <TTree.h>
#include <TChain.h>
#include <TFile.h>
#include <iostream>
#include <vector>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>
#pragma link C++ class vector<THSParticle* >+;//want to make tree branch

#include "THSParticle.h"
#include "THSWeights.h"

class THSDataManager{

 public :
  THSDataManager(){};
  virtual  ~THSDataManager();

  virtual Bool_t Init(TString filename,TString name="");
  Bool_t InitTreeReader(TString filename,TString name);
  virtual Bool_t ReadEvent(Long64_t entry=-1);
  virtual void PrintEvent(Long64_t=0);
  virtual void WriteParticles(TString filename);
  virtual void PostWrite(){} //in case want to reset anything after write
  
  Int_t GetN(){return fNin;}
  THSParticle* GetParticle(Int_t i){return fParticles.at(i);}
  THSParticle* GetGenerated(Int_t i){return fGenerated.at(i);}
  vector<THSParticle*>* GetParticles(){return &fParticles;}
  vector<THSParticle*>* GetGenerated(){return &fGenerated;}
  void AddParticle(THSParticle* part){fParticles.push_back(part);fNin++;};
  void AddGenerated(THSParticle* part){fGenerated.push_back(part);fNgen++;};
  void ClearParticles(){fParticles.clear();fGenerated.clear();fNin=0;fNgen=0;};
  
  void SetReadBranch(TString name){fReadBName=name;}
  void SetReadGenBranch(TString name=""){fReadGName=name;fInGenerated=kTRUE;}
  void SetWriteGenBranch(TString name=""){fWriteGName=name;fAddGenerated=kTRUE;}
  virtual void InitOutput(TString filename);
  virtual void WriteEvent(){fWriteTree->Fill();}
  virtual void CloseOutput();
  virtual void ReadWriteChain(TChain* chain,TString OutDirName,TString FileAppend);
  virtual void CloseReadTree();
  
  void AddUID(TString filename,TString treename="HSParticles"); //Add branch with unique ID to the HSParticles tree
  void SetUID(Long64_t val){UID=val;}

  Long64_t GetEntry(){return fEntry;}
 protected :
  //GENERAL VECTORS FOR READING/WRITING
  vector<THSParticle*> fParticles; //vector of  particles
  vector<THSParticle*> * fReadParticles=nullptr; //vector of input particles from some source
  vector<THSParticle*> fGenerated; //vector of generated particles
  vector<THSParticle*> * fReadGenerated=nullptr; //vector of input particles from some source
  UInt_t fNin=0; //number of iinput particles
  UInt_t fNgen=0; //number of generated particles

  //default THSParticles tree reader
  TFile* fReadFile=0;
  TTree* fReadTree =0;
  Long64_t fEntry=0;
  TString fReadBName="Particles"; //default branchname
  TString fReadGName=""; //by default no gnereated branch
  TString fWriteGName=""; //by default no gnereated branch
  TString fIDName="UID"; //by default uid
  Bool_t fAddGenerated=kFALSE; //also generated vector?
  Bool_t fInGenerated=kFALSE; //also generated vector?
  //default THSParticles tree writer
  Bool_t fWriteThis=kTRUE; //write this event?
  TFile* fWriteFile=0;
  TTree* fWriteTree=0;
  
  //VECTORS (TTREEREADERARRAYS) FOR DEFAULT TREE READING
  TTreeReader fTReader;
  TTreeReaderArray<THSParticle> fTParticles{fTReader, "Particles"};
  TTreeReaderArray<THSParticle>* fTGenerated=nullptr;
  TTreeReaderValue<Double_t>* fTUID=nullptr;

  Double_t UID=0;//needs to be double for RooFit

  //Weight handling
  THSWeights* fWeights=nullptr; //weights tree
  Double_t fWeight=1; //weight for set species for current event
 
 public :
  TTreeReaderArray<THSParticle>* GetReaderParticles(){return &fTParticles;}
  TTreeReaderArray<THSParticle>* GetReaderGenerated(){return fTGenerated;}

  void LoadWeights(TString fname,TString wname);
  void GetWeightEvent();
  Double_t GetWeight(Int_t species);
  THSWeights* GetWeights(){return fWeights;}
  TTree* GetReadTree(){return fReadTree;}

};

#endif //ifdef THSDATAREADER
