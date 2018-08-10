/**
 * \class THSTest
 * 
 * Template final class .
 * 
 * Users should create their own analysis specific project classes.
 * 
 */



#include "TDatabasePDG.h"
#include "THSTest.h"
#include <algorithm>


THSTest::THSTest(){
  SetVerbose(1);
  CheckCombi(); //comment out to remove messages
  
  //Set final state detected particles
  //AddParticle(particle,true/false you want to write in final vector, genID for linking to generated truth value)
  //Note if particle is added to final with a valid genID it will be used
  //to determine the correct permutation of the simulated event
  AddParticle(&fBeam,kTRUE,-1);
  AddParticle(&fGamma1,kTRUE,-1);
  AddParticle(&fGamma2,kTRUE,-1);
  AddParticle(&fNeutron,kTRUE,-1);

  //Set final state parents
  AddParticle(&fPi0,kTRUE,-1);
  ConfigParent(&fPi0,&fGamma1);
  ConfigParent(&fPi0,&fGamma2);
  
  
  
  TString PID("Beam"); //set this to which particles you want to id via pdg code alone, or set it in individual AddTopology
  TString INCLUSIVE("");//set this to which particles you want reaction to be inclusive of, or set it in individual AddTopology "ALL"=> completely inclusive

  //include topology for analysis and get index
  AddTopology("Beam:gamma:gamma:gamma",
             bind(&THSTest::Init_Iter0, this),
             bind(&THSTest::Topo_0, this),
             PID,INCLUSIVE);

  
  
  THSFinalState::InitFS();
}

void THSTest::FileStart(){
  //May be called when a new file is opened
  
}

//Define topology Iterator functions
void THSTest::Init_Iter0(){
  //THSParticle iterator initialisation
  //For topology Beam:neutron:gamma:gamma

   AutoIter(); //Let finalstate try and work out the iterattor for you, you can remove this if you want to do it yourself
}
// void THSTest::Init_IterX(){
//  THSParticleIter* diter=CreateParticleIter(fTID_X,&fVecY,NPARTICLES);
//  ...
// }
//Define topology functions
void THSTest::Topo_0(){
  //For topology Beam:neutron:gamma:gamma

  //Reconstruct missing or combined particles
  //HSLorentzVector miss=fBeam+fTarget-fElectron.P4()...;
  //fMissMass2=miss.M2();
  //fMissMass=miss.M();
}
// void THSTest::Topo_X(){
// }

void THSTest::Kinematics(){
  //Do calculations if Good Event

}
//////////////////////////////////////////////////
/// Define conditions for track to be considered
/// good in event. Adding conditions on junk tracks
///  can greatly reduce combitorials etc.
/// kFALSE=> track ignored completely
Bool_t THSTest::CheckParticle(THSParticle* part){
  return kTRUE;
}

void THSTest::FinalStateOutTree(TTree* tree){
  THSFinalState::fFinalTree=tree;
  //tree->Branch("Final",&fFinal);//If you want to save the final THSParticles
  tree->Branch("MissMass",&fMissMass,"MissMass/D");
  tree->Branch("Topo",&fTopoID,"Topo/I");
  tree->Branch("NPerm",&fNPerm,"NPerm/I");
  tree->Branch("NDet",&fNDet,"NDet/I");

}
