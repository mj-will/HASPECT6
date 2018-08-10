/**
 * \class THSK0L
 * 
 * Template final class .
 * 
 * Users should create their own analysis specific project classes.
 * 
 */



#include "TDatabasePDG.h"
#include "THSK0L.h"
#include <algorithm>


THSK0L::THSK0L(){
  SetVerbose(1);
  //CheckCombi(); //comment out to remove messages
  
  //Set final state detected particles
  //AddParticle(particle,true/false you want to write in final vector, genID for linking to generated truth value)
  //Note if particle is added to final with a valid genID it will be used
  //to determine the correct permutation of the simulated event
  //AddParticle(&fSpectator,kTRUE,4);
  AddParticle(&fBeam,kTRUE,-1);
  AddParticle(&fProton,kTRUE,0);
  AddParticle(&fPip,kTRUE,2);
  AddParticle(&fPimL,kTRUE,1);
  AddParticle(&fPimK,kTRUE,3);

  //Set final state parents
  AddParticle(&fLambda,kTRUE,-1);
  ConfigParent(&fLambda,&fProton);
  ConfigParent(&fLambda,&fPimL);
  AddParticle(&fK0,kTRUE,-1);
  ConfigParent(&fK0,&fPip);
  ConfigParent(&fK0,&fPimK);
  
  
  
  TString PID("NONE"); //set this to which particles you want to id via pdg code alone, or set it in individual AddTopology
  TString INCLUSIVE("ALL");//set this to which particles you want reaction to be inclusive of, or set it in individual AddTopology "ALL"=> completely inclusive

  //include topology for analysis and get index
  AddTopology("proton:pi+:pi-:pi-:Beam",
             bind(&THSK0L::Init_Iter0, this),
             bind(&THSK0L::Topo_0, this),
             PID,INCLUSIVE);

  // AddTopology("Beam:proton:pi+:pi-:pi-:proton",
  //            bind(&THSK0L::Init_Iter1, this),
  //            bind(&THSK0L::Topo_1, this),
  //            PID,INCLUSIVE);

  
  
  THSFinalState::InitFS();
}

void THSK0L::FileStart(){
  //May be called when a new file is opened
  fTrigger.SetParticles(frDetParts);
  fTrigger.SetEventInfo(fEventInfo);
  fTrigger.SetRunInfo(fRunInfo);
  //fTrigger.SetSim(); //Check if this file is simulated or real
}

//Define topology Iterator functions
void THSK0L::Init_Iter0(){
  //THSParticle iterator initialisation
  //For topology Beam:proton:pi+:pi-:pi-

   AutoIter(); //Let finalstate try and work out the iterattor for you, you can remove this if you want to do it yourself
}
void THSK0L::Init_Iter1(){
  //THSParticle iterator initialisation
  //For topology Beam:proton:pi+:pi-:pi-:proton

   AutoIter(); //Let finalstate try and work out the iterattor for you, you can remove this if you want to do it yourself
}
// void THSK0L::Init_IterX(){
//  THSParticleIter* diter=CreateParticleIter(fTID_X,&fVecY,NPARTICLES);
//  ...
// }
//Define topology functions
void THSK0L::Topo_0(){
  if(frGenParts)fTrigger.SetSim(kTRUE);
  else fTrigger.SetSim(kFALSE);
  if(IsMissing(&fProton)) {fGoodEvent=kFALSE;return;}
  if(IsMissing(&fPip)) {fGoodEvent=kFALSE;return;}
  fTrigger.SubtractStartTime(&fPip,&fProton,&fPimL,&fPimK);
  fTrigger.SubtractStartTimeBeam(&fPip,&fBeam);

  //For topology Beam:proton:pi+:pi-:pi-
  //Reconstruct missing or combined particles
  HSLorentzVector miss=fBeam.P4()+fTarget-fProton.P4()-fPip.P4()-fPimK.P4()-fPimL.P4();
  fMissMass=miss.M();
}
void THSK0L::Topo_1(){
  //For topology Beam:proton:pi+:pi-:pi-:proton

  //Reconstruct missing or combined particles
  //HSLorentzVector miss=fBeam+fTarget-fElectron.P4()...;
  //fMissMass2=miss.M2();
  //fMissMass=miss.M();
}
// void THSK0L::Topo_X(){
// }

void THSK0L::Kinematics(){
  //Do calculations if Good Event
  fK0.Add(&fPimK,&fPip,311);
  fLambda.Add(&fPimL,&fProton,3122);
  fReaction.Add(&fK0,&fLambda);
  fKine.SetMesonBaryon(fK0.P4(),fLambda.P4());

  fMK0=fK0.MassDiff();
  fML=fLambda.MassDiff();

 //Linear Polarisation
  fPol=fBeam.Vertex().R();
  if(fBeam.Vertex().X()) fPolState=1;
  else if(fBeam.Vertex().Y()) fPolState=-1;
  else fPolState=0;

   //Spectator
  HSLorentzVector spec=fBeam.P4()+fTarget-fK0.P4()-fLambda.P4();
  fMissMassFix=spec.M();
  fSpecP= spec.P();
  fSpecTh= TMath::Cos(spec.Theta());
  //target neutron momentum = -spectator
  HSLorentzVector tarN=fKine.ParticleXYZM(-spec.X(),-spec.Y(),-spec.Z(),0.939565378);
 //CM
  f_W=fReaction.P4p()->M();
  fEgamma=fBeam.P4p()->E();
  fKine.SetGammaTarget(fBeam.P4(),tarN);
  fKine.PhotoCMDecay();
  fCMCosTh=fKine.CosTheta();
  fCMPhi=fKine.Phi();
  fK0Phi=fK0.P4p()->Phi();
  fKine.PolPhotoCMDecay();
  fPolCosTh=fKine.CosTheta();
  fPolPhi=fKine.Phi();
  //Lmabda Deczy
  fKine.SetBaryonDecay(fProton.P4(),fPimL.P4());
  fKine.LambdaDecay();
  fCosx=fKine.Cosx();
  fCosy=fKine.Cosy();
  fCosz=fKine.Cosz();
  if(!frGenParts) return;
  tarN=fKine.ParticleXYZM(-spec.X(),-spec.Y(),-spec.Z(),0.939565378);;
  fKine.SetGammaTarget(fBeam.P4(),tarN);
  HSLorentzVector pK0=fPimK.TruthP4()+fPip.TruthP4();
  HSLorentzVector pL=fPimL.TruthP4()+fProton.TruthP4();
  fKine.SetMesonBaryon(pK0,pL);
  fKine.PhotoCMDecay();
  genfCMCosTh=fKine.CosTheta();
  genfCMPhi=fKine.Phi();
  fKine.PolPhotoCMDecay();
  genfPolCosTh=fKine.CosTheta();
  genfPolPhi=fKine.Phi();
   //Lmabda Deczy
  fKine.SetBaryonDecay(fProton.TruthP4(),fPimL.TruthP4());
  fKine.LambdaDecay();
  genfCosx=fKine.Cosx();
  genfCosy=fKine.Cosy();
  genfCosz=fKine.Cosz();

}
//////////////////////////////////////////////////
/// Define conditions for track to be considered
/// good in event. Adding conditions on junk tracks
///  can greatly reduce combitorials etc.
/// kFALSE=> track ignored completely
Bool_t THSK0L::CheckParticle(THSParticle* part){
  return kTRUE;
}

void THSK0L::FinalStateOutTree(TTree* tree){
  THSFinalState::fFinalTree=tree;
  //tree->Branch("Final",&fFinal);//If you want to save the final THSParticles
  tree->Branch("Correct",&fCorrect,"Correct/I");
  tree->Branch("Topo",&fTopoID,"Topo/I");
  tree->Branch("NPerm",&fNPerm,"NPerm/I");
  tree->Branch("NDet",&fNDet,"NDet/I");
  tree->Branch("FidCut",&fFidCut,"FidCut/I");
  tree->Branch("MissMass",&fMissMass,"MissMass/D");
  tree->Branch("MissMassFix",&fMissMassFix,"MissMassFix/D");
  // tree->Branch("MeasMass",&fMeasMass,"MeasMass/D");
  tree->Branch("MK0",&fMK0,"MK0/D");
  tree->Branch("ML",&fML,"ML/D");
  tree->Branch("Cosx",&fCosx,"Cosx/D");
  tree->Branch("Cosy",&fCosy,"Cosy/D");
  tree->Branch("Cosz",&fCosz,"Cosz/D");
  tree->Branch("Egamma",&fEgamma,"Egamma/D");
  tree->Branch("CosTh",&fCMCosTh,"CosTh/D");
  tree->Branch("SpecTh",&fSpecTh,"SpecTh/D");
  tree->Branch("SpecP",&fSpecP,"SpecP/D");
  tree->Branch("K0Phi",&fK0Phi,"K0Phi/D");
  tree->Branch("Phi",&fCMPhi,"Phi/D");
  tree->Branch("PolPhi",&fPolPhi,"PolPhi/D");
  tree->Branch("PolCosTh",&fPolCosTh,"PolCosTh/D");

  if(!frGenParts){//Linear pol only for data
    tree->Branch("Pol",&fPol,"Pol/D");
    tree->Branch("PolState",&fPolState,"PolState/I");
  }
  else { //generated branches only for simulation
    tree->Branch("Correct",&fCorrect,"Correct/I");
    tree->Branch("genCosx",&genfCosx,"genCosx/D");
    tree->Branch("genCosy",&genfCosy,"genCosy/D");
    tree->Branch("genCosz",&genfCosz,"genCosz/D");
    tree->Branch("genPhi",&genfCMPhi,"genPhi/D");
    tree->Branch("genPolPhi",&genfPolPhi,"genPolPhi/D");
    tree->Branch("genPolCosTh",&genfPolCosTh,"genPolCosTh/D");
    //tree->Branch("PolPhi",&fPolPhi,"PolPhi/D");
  }
}
