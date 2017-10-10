
#ifndef THSFINALSTATE_h
#define THSFINALSTATE_h

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


class THSFinalState{

 public :
  THSFinalState() {};
  // THSFinalState(TString name,TString title):TNamed(name,title){};
  virtual ~THSFinalState(){};

  virtual void GetEvent(Long64_t uid){}//interface to different readers
  virtual Bool_t WorkOnEvent(){return kFALSE;};
  virtual void FinaliseEvent(){};
  virtual void InitEvent(){fGotCorrectOne=kFALSE;  fNPerm=0;};
  
  void SetDetParts(vector<THSParticle*> * dpp){frDetParts=dpp;}
  void SetGenParts(vector<THSParticle*> * dpp){frGenParts=dpp;}

  Int_t AddTopology(TString topo);
  vector<Int_t> GetTopology(Int_t i){return fTopo.at(i);};
  Int_t FindTopology(); //For current event
  Int_t FindInclusiveTopology(Int_t incType=-1E9);//Inclusive For current event
  void SetPermutate(){fTryPerm=kTRUE;} //Turn on permuations
  Bool_t RotatePartVector(vector<THSParticle*>* vec,Int_t *Nturns);

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
  Bool_t fGotCorrectOne=kFALSE;
  
  //Combitorial
  Bool_t fTryPerm=kFALSE;
  Int_t fNPerm=0;
  void InitParticles(); //Allocate particles to vector for this event
  void InitDetParts(Int_t pdg,vector<THSParticle*> *parts); //Fill vector
  Bool_t PermutateParticles(); //Permutate particles in vectors
  vector<THSParticle*> fVecProtons;
  vector<THSParticle*> fVecPiPs;
  vector<THSParticle*> fVecPiMs;
  vector<THSParticle*> fVecKPs;
  vector<THSParticle*> fVecKMs;
  vector<THSParticle*> fVecEls;
  vector<THSParticle*> fVecPos;
  vector<THSParticle*> fVecPi0s;
  vector<THSParticle*> fVecGams;
  vector<THSParticle*> fVecPlus;
  vector<THSParticle*> fVecMinus;
  vector<THSParticle*> fVec0;
  vector<THSParticle*> fVecBeams;
  vector<Int_t> fDetTypes;
  // map<Int_t, vector<THSParticle> > fPVecs;
  Bool_t fIsPermutating0=kFALSE;
  Bool_t fIsPermutating1=kFALSE;
  Int_t fNProtTurns=0;
  Int_t fNPipTurns=0;
  Int_t fNPimTurns=0;
  Int_t fNPi0Turns=0;
  Int_t fNKpTurns=0;
  Int_t fNKmTurns=0;
  Int_t fNElTurns=0;
  Int_t fNPosTurns=0;
  Int_t fNPlusTurns=0;
  Int_t fNMinusTurns=0;
  Int_t fN0Turns=0;
  Int_t fNBeamTurns=0;
  Int_t fNGamTurns=0;

  TTree* fFinalTree=nullptr;
  
  //Kinematics calculator
  THSKinematics fKine;

  //Unique event ID
  Double_t fUID=0;
  
  //Detected Particles
  vector<THSParticle* >* frDetParts=nullptr;
  vector<THSParticle* >* frGenParts=nullptr;
  //TTreeReaderArray<THSParticle> *frDetParts=nullptr;
  //TTreeReaderArray<THSParticle> *frGenParts=nullptr;

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
  virtual void FinalStateOutTree(TTree* tree){tree->Branch("Final",&fFinal);};
  virtual void CheckTruth();

};

#endif //ifdef THSPROJECT
