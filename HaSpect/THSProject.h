//Class to control particlular analysis
//Users need to create their own specific project
//It should operate on 3 classes of data:
//real, mc rec, mc gen
//input should be in form of events extracted
//from trees or text files,...
/////////////////////////////////////
//The main task is to take detected particles and
//convert them into information needed for subsequent
//fitting of observables, e.g TLorentzVectors for AmpTools
//Double_t s for RooFit

#ifndef THSPROJECT_h
#define THSPROJECT_h

#include <TString.h>
//#include <TNamed.h>
#include <TTree.h>
#include <vector>
#include <map>

#include "THSParticle.h"
#include "THSDataManager.h"
#include "THSKinematics.h"
#include <TTreeReader.h>
#include <TTreeReaderValue.h>
#include <TTreeReaderArray.h>


class THSProject{

 public :
  THSProject() {};
  // THSProject(TString name,TString title):TNamed(name,title){};
  virtual ~THSProject(){};

  virtual void GetEvent(Long64_t uid){fUID=uid;}//interface to different readers
  virtual Bool_t WorkOnEvent(){return kFALSE;};
  virtual void FinaliseEvent(){};//inerface to different writers
  
  void SetDetParts(TTreeReaderArray<THSParticle> * dpp){frDetParts=dpp;}
  void SetGenParts(TTreeReaderArray<THSParticle> * dpp){frGenParts=dpp;}

  Int_t AddTopology(TString topo);
  vector<Int_t> GetTopology(Int_t i){return fTopo.at(i);};
  Int_t FindTopology(); //For current event
  Int_t FindInclusiveTopology(Int_t incType=-1E9);//Inclusive For current event
  void SetPermutate(){fTryPerm=kTRUE;} //Turn on permuations

 protected :
  vector<vector<Int_t> >fTopo; //vector of topologies
  Int_t fNTopo=0; //Number of added topologies
  Int_t fCurrTopo=-1;
  UInt_t fNParts=0; //Number of particles detected
  UInt_t fNGen=0; //Number of generated particles
  Bool_t fGoodEvent = kTRUE;

  //Special behaviour for generated MC events
  Bool_t fIsGenerated=kFALSE;
  virtual void InitGenerated(){}
 //For simulated events flag with correct permutation
  Int_t fCorrect=kTRUE;

  //Combitorial
  Bool_t fTryPerm=kFALSE;
  void InitParticles(); //Allocate particles to vector for this event
  void InitDetParts(Int_t pdg,vector<THSParticle> *parts); //Fill vector
  Bool_t PermutateParticles(); //Permutate particles in vectors
  vector<THSParticle> fVecProtons;
  vector<THSParticle> fVecPiPs;
  vector<THSParticle> fVecPiMs;
  vector<THSParticle> fVecKPs;
  vector<THSParticle> fVecKMs;
  vector<THSParticle> fVecEls;
  vector<THSParticle> fVecPos;
  vector<THSParticle> fVecPi0s;
  vector<THSParticle> fVecGams;
  vector<THSParticle> fVecPlus;
  vector<THSParticle> fVecMinus;
  vector<THSParticle> fVec0;
  vector<THSParticle> fVecBeams;
  vector<Int_t> fDetTypes;
  // map<Int_t, vector<THSParticle> > fPVecs;
  Bool_t fIsPermutating0=kFALSE;
  Bool_t fIsPermutating1=kFALSE;

  TTree* fFinalTree=nullptr;
  //Kinematics calculator
  THSKinematics fKine;

  //Unique event ID
  Double_t fUID=0;
  //Detected Particles
  //vector<THSParticle* >* fDetParts=nullptr;
  TTreeReaderArray<THSParticle> *frDetParts=nullptr;
  TTreeReaderArray<THSParticle> *frGenParts=nullptr;

  //Final Particles vector for saving in output tree
  vector<THSParticle*> fFinal;
 
  //Observables

  //Discriminators
  
 public :
  void SetGenerated(Bool_t gen=kTRUE){fIsGenerated=kTRUE;}
  Bool_t IsGoodEvent(){return fGoodEvent;}
  virtual Bool_t IsPermutating(){return fTryPerm*(fIsPermutating0+fIsPermutating1);}
  virtual void ProcessEvent();
  virtual void MatchWithGen(THSParticle* part);  
  virtual Bool_t IsCorrectTruth(THSParticle *part);
  virtual void ProjectOutTree(TTree* tree){tree->Branch("Final",&fFinal);};
  virtual void CheckTruth();

};

#endif //ifdef THSPROJECT
