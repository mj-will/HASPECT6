/**
 * \class THSep_omegap
 * 
 * Template final class .
 * 
 * Users should create their own analysis specific project classes.
 * 
 */



#include "TDatabasePDG.h"
#include "THSep_omegap.h"
#include <algorithm>


THSep_omegap::THSep_omegap(){
  SetVerbose(1);
  // CheckCombi(); //comment out to remove messages
  
  //Set final state detected particles
  //AddParticle(particle,true/false you want to write in final vector, genID for linking to generated truth value)
  //Note if particle is added to final with a valid genID it will be used
  //to determine the correct permutation of the simulated event
  AddParticle(&fElectron,kTRUE,4);
  AddParticle(&fProton,kTRUE,3);
  AddParticle(&fPip,kTRUE,0);
  AddParticle(&fPim,kTRUE,1);
  AddParticle(&fGamma1,kTRUE,-1);
  AddParticle(&fGamma2,kTRUE,-1);

  //Set final state parents
  AddParticle(&fPi0,kTRUE,2);
  ConfigParent(&fPi0,&fGamma1);
  ConfigParent(&fPi0,&fGamma2);
  AddParticle(&fOmega,kTRUE,-1);
  ConfigParent(&fOmega,&fPip);
  ConfigParent(&fOmega,&fPim);
  ConfigParent(&fOmega,&fPi0);
   
  
  TString PID("NONE"); //set this to which particles you want to id via pdg code alone, or set it in individual AddTopology
  TString INCLUSIVE("ALL");//set this to which particles you want reaction to be inclusive of, or set it in individual AddTopology "ALL"=> completely inclusive

  //include topology for analysis and get index
  AddTopology("pi+:pi-:gamma:gamma:proton:e-",
             bind(&THSep_omegap::Init_Iter0, this),
             bind(&THSep_omegap::Topo_0, this),
             PID,INCLUSIVE);

  AddTopology("pi+:pi-:proton:e-",
             bind(&THSep_omegap::Init_Iter1, this),
             bind(&THSep_omegap::Topo_1, this),
             PID,INCLUSIVE);

  
  
  THSFinalState::InitFS();
}

void THSep_omegap::FileStart(){
  //May be called when a new file is opened
  fTrigger.SetParticles(frDetParts);
  fTrigger.SetEventInfo(fEventInfo);
  fTrigger.SetRunInfo(fRunInfo);

  //Configure the cuts (default are delta timing cuts in ns)
  fCuts.SetElCut(2,2); //Just FT,FD
  fCuts.SetPionPCut(0,2,0,2);//FT,FDTOF,CD,FDCAL
  fCuts.SetPionMCut(0,2,0,2);//FT,FDTOF,CD,FDCAL
  fCuts.SetProtCut(0,2,1,2);//FT,FDTOF,CD,FDCAL
  fCuts.SetGammaCut(2,2,0,2);//FT,FDTOF,CD,FDCAL
  
}

//Define topology Iterator functions
void THSep_omegap::Init_Iter0(){
  //THSParticle iterator initialisation
  //For topology pi+:pi-:gamma:gamma:proton:e-

   AutoIter(); //Let finalstate try and work out the iterattor for you, you can remove this if you want to do it yourself
}
void THSep_omegap::Init_Iter1(){
  //THSParticle iterator initialisation
  //For topology pi+:pi-:proton:e-

   AutoIter(); //Let finalstate try and work out the iterattor for you, you can remove this if you want to do it yourself
}
// void THSep_omegap::Init_IterX(){
//  THSParticleIter* diter=CreateParticleIter(fTID_X,&fVecY,NPARTICLES);
//  ...
// }
//Define topology functions
void THSep_omegap::Topo_0(){
  //For topology pi+:pi-:gamma:gamma:proton:e-
  //if(fElectron.Detector()>0) {fGoodEvent=kFALSE;return;} //Put some cuts on particle detectors
  //Define starttime from electron candidate
  fTrigger.StartTime(&fElectron);
  //Subtract sarttime from all particles
  //e.g. fTrigger.SubtractStartTime(&fElectron,&fProton,&fPip,&fPim);
  fTrigger.SubtractStartTime(&fElectron,&fProton,&fPip,&fPim,&fGamma1,&fGamma2);

  //Can apply some timing cuts now
  if(!fCuts.ElCut(&fElectron)){fGoodEvent=kFALSE;return;}
  if(!fCuts.ProtCut(&fProton)){fGoodEvent=kFALSE;return;}
  if(!fCuts.PionPCut(&fPip)){fGoodEvent=kFALSE;return;}
  if(!fCuts.PionMCut(&fPim)){fGoodEvent=kFALSE;return;}
  if(!fCuts.GammaCut(&fGamma1)){fGoodEvent=kFALSE;return;}
  if(!fCuts.GammaCut(&fGamma2)){fGoodEvent=kFALSE;return;}

  fPi0.Add(&fGamma1,&fGamma2,fPi0.PDG());
  fPi0Mass=fPi0.MassDiff();
  if(TMath::Abs(fPi0Mass)>0.05){fGoodEvent=kFALSE;return;}
 //Reconstruct missing or combined particles
  HSLorentzVector miss=fBeam+fTarget-fElectron.P4()-fProton.P4()-fPip.P4()-fPim.P4()-fPi0.P4();
  fMissMass2=miss.M2();
  fMissMass=miss.M();
}
void THSep_omegap::Topo_1(){
  //For topology pi+:pi-:proton:e-
  //if(fElectron.Detector()>0) {fGoodEvent=kFALSE;return;} //Put some cuts on particle detectors
  //Define starttime from electron candidate
  fTrigger.StartTime(&fElectron);
  //Subtract sarttime from all particles
  //e.g. fTrigger.SubtractStartTime(&fElectron,&fProton,&fPip,&fPim);
  fTrigger.SubtractStartTime(&fElectron,&fProton,&fPip,&fPim,&fGamma1,&fGamma2);

  //Can apply some timing cuts now
  if(!fCuts.ElCut(&fElectron)){fGoodEvent=kFALSE;return;}
  if(!fCuts.ProtCut(&fProton)){fGoodEvent=kFALSE;return;}
  if(!fCuts.PionPCut(&fPip)){fGoodEvent=kFALSE;return;}
  if(!fCuts.PionMCut(&fPim)){fGoodEvent=kFALSE;return;}
 //Reconstruct missing or combined particles
  HSLorentzVector miss=fBeam+fTarget-fElectron.P4()-fProton.P4()-fPip.P4()-fPim.P4();
  fMissMass2=miss.M2();
  fMissMass=miss.M();
}
// void THSep_omegap::Topo_X(){
// }

void THSep_omegap::Kinematics(){
  //configure trigger for this event
  fTrigger.ReadParticles();
  //Do calculations if Good Event
  HSLorentzVector missP=fBeam+fTarget-fElectron.P4()-fProton.P4();
  fMissMassP=missP.M();
  //invariant mass of 3pi
  fOmega.SetP4(fPip.P4()+fPim.P4()+fPi0.P4());
  fOmega.SetMeasMass(fOmega.P4p()->M());
  fOmega.TakePDGMass();
  fOMass=fOmega.MassDiff();
  
  //Set the initial and scattered electron and target 4-vectors
  fKine.SetElecsTarget(fBeam,fElectron.P4(),fTarget);
  //set the final state meson and baryon 4-vectors
  fKine.SetMesonBaryon(fOmega.P4(),fProton.P4());
  //Get electron scattering variables
  f_W=fKine.W();
  f_Q2=fKine.Q2();
  f_Pol=fKine.GammaPol();
  //Get Omega production variables
  fKine.ElectroCMDecay();
  fCMCosTh=fKine.CosTheta();
  fCMPhi=fKine.Phi();
  f_t=fKine.t(); //t for Omega meson production

  //Get omega decay variables
  fKine.SetMesonDecay(fPip.P4(),fPim.P4());
  fKine.omegaMesonDecayHelicity();
  fHelCosTh=fKine.CosTheta();
  fHelPhi=fKine.Phi();

  
}
//////////////////////////////////////////////////
/// Define conditions for track to be considered
/// good in event. Adding conditions on junk tracks
///  can greatly reduce combitorials etc.
/// kFALSE=> track ignored completely
Bool_t THSep_omegap::CheckParticle(THSParticle* part){
  if(part->Detector()<0)return kTRUE; //All FT tracks
  if(part->PDG()==45)return kFALSE; //? what are these?
  if(part->Time()==0)return kFALSE;   //Track needs time
  if(part->Charge()&&part->DeltaE()<2)return kFALSE; //Charged track needs deltaE>2

  return kTRUE;
}

void THSep_omegap::FinalStateOutTree(TTree* tree){
  THSFinalState::fFinalTree=tree;
  //tree->Branch("Final",&fFinal);//If you want to save the final THSParticles
  tree->Branch("MissMass",&fMissMass,"MissMass/D");
  tree->Branch("Topo",&fTopoID,"Topo/I");
  tree->Branch("NPerm",&fNPerm,"NPerm/I");
  tree->Branch("NDet",&fNDet,"NDet/I");
  tree->Branch("OMass",&fOMass,"OMass/D");
  tree->Branch("Pi0Mass",&fPi0Mass,"Pi0Mass/D");
  tree->Branch("t",&f_t,"t/D");
  tree->Branch("Q2",&f_Q2,"Q2/D");
  tree->Branch("W",&f_W,"W/D");
  tree->Branch("Pol",&f_Pol,"Pol/D");
  tree->Branch("CMCosTh",&fCMCosTh,"CMCosTh/D");
  tree->Branch("CMPhi",&fCMPhi,"CMPhi/D"); 
  tree->Branch("HelCosTh",&fHelCosTh,"HelCosTh/D");
  tree->Branch("HelPhi",&fHelPhi,"HelPhi/D"); 

}
