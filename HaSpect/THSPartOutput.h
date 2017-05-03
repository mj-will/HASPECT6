//--Author      DI Glazier 13/01/2017
//--Rev
//--Update
//--Description
//HASPECT Event Reconstruction
//THSPartOutput
//Controls the output of the TSelector analysis
//Creates THSParticle branches in ouput tree
//fStrParticles should be passed a string of particles to be
//searched for and output in the tree, e.g. "pi+:pi-:pi+:neutron"
//the particles will be ordered in fDetParticle[i] for further analysis
//ordered by type then decreasing momentum, see output message for connecting
//branch names to fDetParticle indices
//After making a skeleton selector class you can add inheritance from this class
//to obtain its functionality

#ifndef THSPartOutput_h
#define THSPartOutput_h
#include "THSParticle.h"
#include "THSOutput.h"
#include <vector>

class THSParticle;

class THSPartOutput : public THSOutput{

 public :
 THSPartOutput() : THSOutput() {};
  ~THSPartOutput();
  
 protected :
  vector<Int_t> fFinalState; //vector holding the particle ids selected in final state
  vector<UInt_t> fNtype; //vector holding the number of different particles in fFinalState
  vector<UInt_t> fIDtype; //vector holding the PDG_ID of different particles in fFinalState
  UInt_t fNdet; //number of particles detected (the size of fFinalState)
  vector<Int_t> fDetID;
  vector<THSParticle*> fDetParticle;

 public :
  THSParticle* GetHSParticle(Int_t np){return fDetParticle[np];}
  vector<UInt_t> GetNType(){return fNtype;}
  vector<Int_t> GetFinalState(){return fFinalState;}
  
  virtual void InitOutput(); //Configure the output file and tree with THSParticles 



};
#endif

