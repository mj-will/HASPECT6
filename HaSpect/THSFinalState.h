
#ifndef THSFINALSTATE_h
#define THSFINALSTATE_h

#include <TString.h>
//#include <TNamed.h>
#include <TTree.h>
#include <vector>
#include <functional>
#include <map>


#include "THSParticle.h"
#include "THSParticleIter.h"
#include "THSDataManager.h"
#include "THSKinematics.h"
/* #include <TTreeReader.h> */
/* #include <TTreeReaderValue.h> */
/* #include <TTreeReaderArray.h> */

namespace FinalState{
  typedef std::function<void()> VoidFuncs;
}
//Developments :
//Add THSParticleIter class for combitorials
//To limit number of combitorials
//First loop over beam particles, assign status of other particles
//based on timing coincidence (0=reject;1=coincidence;2=random sample)
//Run check topology based on status
class THSTopology;
class THSParticleConfig;

class THSFinalState{

 public :
  THSFinalState();
  virtual ~THSFinalState(){fConfigs.clear();};

  virtual void GetEvent(Long64_t uid){}//interface to different readers
  virtual Bool_t WorkOnEvent();
  virtual void FinaliseEvent(){};
  virtual void InitEvent(){fGotCorrectOne=kFALSE;  fNPerm=0; fTopoID=-1;fNTried=0;};
  virtual void InitFS();

  virtual void InitGenerated();
  virtual void InitDetected();
  virtual void Kinematics(){return;};
  
  void SetDetParts(vector<THSParticle*> * dpp){frDetParts=dpp;}
  void SetGenParts(vector<THSParticle*> * dpp){frGenParts=dpp;}

  void AddTopology(TString topo,FinalState::VoidFuncs funcI,FinalState::VoidFuncs funcE,TString chPID="",TString incl="");
  THSTopology* FindTopology(); //For current event

  void AddParticle(THSParticle* part,Bool_t AddToFinal,Int_t genID);
  void ConfigParent(THSParticle* parent,THSParticle* child);
  vector<THSParticleConfig* > HowManyParticles(Int_t pdg);
  
  void SetPermutate(){fTryPerm=kTRUE;} //Turn on permuations
  void SetInclusive(TString parts="");
  void SetMaxParticles(UInt_t max){fMaxPart=max;}
  void SetGenerated(Bool_t gen=kTRUE){fIsGenerated=gen;}
  void SetUseChargePID(Bool_t gen=kTRUE){fUseChargePID=gen;}

 protected :
  THSTopology *fCurrTopo=nullptr;
  vector<THSTopology* >fTopos; //vector of topologies
  Int_t fTopoID=0; //Topo ID for this event
  Int_t fNTopo=0; //Number of added topologies
   
  THSParticleIter* fCurrIter=nullptr;
  
  vector<Int_t> fThisTopo; //topology for this event
  vector<Int_t> fIncParts; //particle allowed to be inclusive
  //Int_t fCurrTopo=-1;
  Int_t fCheckTopo=0;
  UInt_t fMaxPart=20;//limit the number of particles allowed in event
  UInt_t fNParts=0; //Number of particles detected
  UInt_t fNGen=0; //Number of generated particles
  Int_t fNDet=0;
  Int_t fNTried=0;
  const Int_t fMISSING=-999999;
  Bool_t fGoodEvent = kTRUE;
  Long64_t fNUsedReadEvent=0; //Number of read events with at least 1 GoodEvent
  //Special behaviour for generated MC events
  Bool_t fIsGenerated=kFALSE;
  
 //For simulated events flag with correct permutation
  Int_t fCorrect=kTRUE;
  Bool_t fGotCorrectOne=kFALSE;
  //Flag to do PID by charge not given pdg code
  Bool_t fUseChargePID=kFALSE;
  Bool_t fIsInclusive=kFALSE;

  Int_t fPrintVerbose=0;
  //Combitorial
  Bool_t IsMissing(THSParticle* part){if(part->Detector()==fMISSING){return kTRUE;}return kFALSE;}
  //  Bool_t IsMissing(THSParticle* part){if(part->P4p()->Theta()==0){return kTRUE;}return kFALSE;}
  Bool_t fTryPerm=kTRUE;
  Bool_t fCheckCombi=kFALSE;
  Int_t fNPerm=0;
  void InitParticles(); //Allocate particles to vector for this event
  void InitDetParts(); //Fill vector based on pdg
  void InitDetPartsCharge(); //Fill vector based on charge
  Bool_t PermutateParticles(); //Permutate particles in vectors
  void CheckParticles();
  void CheckCombitorial();
  
  vector<THSParticleConfig*> fConfigs;
  
  vector<THSParticle*> fVecProtons;
  vector<THSParticle*> fVecAntiProtons;
  vector<THSParticle*> fVecNeutrons;
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

  //  vector<THSParticleIter*> fDetIter;  //Particle iterator for each topology

  Int_t fIterLevel=0;

    
  map< Int_t , vector< THSParticle* >* > fMapPDGtoParticle;
  //map< Int_t , Int_t > fParticleCounter; //count how many of each type of particle have been asked for this permutation

  Bool_t fIsPermutating0=kFALSE;
  Bool_t fIsPermutating1=kFALSE;

  TTree* fFinalTree=nullptr;
  
  //Kinematics calculator
  THSKinematics fKine;

  //Unique event ID
  Double_t fUID=0;
  
  //Detected Particles
  vector<THSParticle* >* frDetParts=nullptr;
  vector<THSParticle* >* frGenParts=nullptr;

  //Final Particles vector for saving in output tree
  vector<THSParticle*> fFinal;

  std::vector<FinalState::VoidFuncs> fTopoFuncs;
  std::vector<FinalState::VoidFuncs> fIterFuncs;
  //Observables

  //Discriminators
  
 public :
  Bool_t IsGoodEvent(){return fGoodEvent;}
  virtual Bool_t IsPermutating(){ return fTryPerm*(fIsPermutating0+fIsPermutating1);}

  virtual void ProcessEvent();
  virtual Bool_t FSProcess();
  virtual void UserProcess();
  
  
  virtual void MatchWithGen(THSParticle* part);  
  virtual Bool_t IsCorrectTruth(THSParticle *part);
  virtual void FinalStateOutTree(TTree* tree){tree->Branch("Final",&fFinal);};
  virtual void CheckTruth();

  void SetVerbose(Int_t ver){fPrintVerbose=ver;}
  void CheckCombi(){fCheckCombi=kTRUE;}
  
 //Particle iterator helpers
  void AutoIter();

  
    THSParticleIter* AddSelectToSelected(THSParticleIter* diter,Int_t ni,Int_t nsel,THSParticle* part0=nullptr,THSParticle* part1=nullptr,THSParticle* part2=nullptr,THSParticle* part3=nullptr,THSParticle* part4=nullptr,THSParticle* part5=nullptr,THSParticle* part6=nullptr,THSParticle* part7=nullptr,THSParticle* part8=nullptr,THSParticle* part9=nullptr);
  
  THSParticleIter* AddSelectToRemainder(THSParticleIter* diter,Int_t ni,Int_t nsel,THSParticle* part0,THSParticle* part1=nullptr,THSParticle* part2=nullptr,THSParticle* part3=nullptr,THSParticle* part4=nullptr,THSParticle* part5=nullptr,THSParticle* part6=nullptr,THSParticle* part7=nullptr,THSParticle* part8=nullptr,THSParticle* part9=nullptr);
  
  THSParticleIter* AddSelectToRemainder(THSParticleIter* diter,Int_t ni,Int_t nsel);

  THSParticleIter* AddSelectXofY(Int_t ni,Int_t nsel,THSParticle* part0=nullptr,THSParticle* part1=nullptr,THSParticle* part2=nullptr,THSParticle* part3=nullptr,THSParticle* part4=nullptr,THSParticle* part5=nullptr,THSParticle* part6=nullptr,THSParticle* part7=nullptr,THSParticle* part8=nullptr,THSParticle* part9=nullptr);
  THSParticleIter* AddSelectToSelected(THSParticleIter* diter,Int_t ni,Int_t nsel,vector<THSParticle*> *parts);
  THSParticleIter* AddSelectXofY(Int_t ni,Int_t nsel,vector<THSParticle*> *parts);
  
  THSParticleIter* AddPermutateToSelected(THSParticleIter* diter,THSParticle* part0=nullptr,THSParticle* part1=nullptr,THSParticle* part2=nullptr,THSParticle* part3=nullptr,THSParticle* part4=nullptr,THSParticle* part5=nullptr,THSParticle* part6=nullptr,THSParticle* part7=nullptr,THSParticle* part8=nullptr,THSParticle* part9=nullptr);

  THSParticleIter* AddPermutateToRemainder(THSParticleIter* diter,THSParticle* part0=nullptr,THSParticle* part1=nullptr,THSParticle* part2=nullptr,THSParticle* part3=nullptr,THSParticle* part4=nullptr,THSParticle* part5=nullptr,THSParticle* part6=nullptr,THSParticle* part7=nullptr,THSParticle* part8=nullptr,THSParticle* part9=nullptr);

  THSParticleIter* AddPermutate(THSParticle* part0=nullptr,THSParticle* part1=nullptr,THSParticle* part2=nullptr,THSParticle* part3=nullptr,THSParticle* part4=nullptr,THSParticle* part5=nullptr,THSParticle* part6=nullptr,THSParticle* part7=nullptr,THSParticle* part8=nullptr,THSParticle* part9=nullptr);
  
  THSParticleIter* CreateParticleIter(vector<THSParticle*> *parts,Int_t Nsel);
  

};
///////////////////////////////////////////////////////
///Utility class for handling THSParticles
class THSParticleConfig{
 public:
  THSParticleConfig(){}
  THSParticleConfig(THSParticle* part,Int_t genID){fParticle=part;fGeni=genID;}
  
  virtual ~THSParticleConfig(){}
  THSParticle* Particle(){return fParticle;}

  vector<THSParticle* >  Children(Int_t pdg);
  void AddChild(THSParticle* child){fChildren.push_back(child);}
  
  THSParticleConfig* Parent(){return fParent;}
  void SetParent(THSParticleConfig* parent){fParent=parent;}

  void SetParticleVal(THSParticle* part){
    fParticle->SetP4(part->P4p());
    fParticle->SetPDGcode(part->PDG());
    fParticle->SetVertex(part->Vertex());
  }
  UInt_t GetNChild(){return fChildren.size();};
  Int_t PDG(){return fParticle->PDG();}
  Int_t GenID(){return fGeni;}


 private:
  THSParticle* fParticle=nullptr;
  THSParticleConfig* fParent=nullptr;
  vector<THSParticle*> fChildren;

 Int_t fGeni=-1; //index in generated events if has one
 
};


#endif //ifdef THSFINALSTATE
