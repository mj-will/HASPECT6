/**
 * \class THSFinalTemp
 * 
 * Template final class .
 * 
 * Users should create their own analysis specific project classes.
 * 
 */



#include "TDatabasePDG.h"
#include "THSFinalTemp.h"
#include <algorithm>


THSFinalTemp::THSFinalTemp(){
  //Set final state

  SetUseChargePID();//If want to only use charge for IDing

  //include topology for analysis and get index
 

 THSFinalState::InitFS();
}


void THSFinalTemp::Init_Generated(){
  if(!THSFinalState::frGenParts) return;
  if(THSFinalState::frGenParts->size()!=REPLACE_WITH_N_GENERATED_PARTICLES) {fGoodEvent=kFALSE;return;}
  //Fill our data member particles
  //User is responsible for indicing right
  //comes from order in generated file (e.g LUND)
  if(THSFinalState::fIsGenerated){
    //fElectron=*frGenParts->at(0);
  }
  else{//Just assign truth values
    //fElectron.SetTruth(frGenParts->at(0));
  }
}

//Define topology Iterator functions
// void THSFinalTemp::Init_IterX(){
//  THSParticleIter* diter=CreateParticleIter(fTID_X,&fVecY,NPARTICLES);
//  ...
// }
//Or if iterator the same as a previous one
// void THSFinalTemp::Init_IterY(){
//   fDetIter[fTID_1]=fDetIter[fTID_0];
// }
//Define topology functions
// void THSFinalTemp::Topo_X(){
// }

void THSFinalTemp::Kinematics(){
  if(!THSFinalState::fGoodEvent) return;//don't do calculations
  //Do calculations if Good Event

}

void THSFinalTemp::FinalStateOutTree(TTree* tree){
  THSFinalState::fFinalTree=tree;
  //tree->Branch("Final",&fFinal);//If you want to save the final THSParticles
  tree->Branch("MissMass",&fMissMass,"MissMass/D");

}
