/**
 * \class THS2pi
 * 
 * Template final class .
 * 
 * Users should create their own analysis specific project classes.
 * 
 */



#include "TDatabasePDG.h"
#include "THS2pi.h"
#include <algorithm>


THS2pi::THS2pi(){
  SetVerbose(1);
  // CheckCombi(); //comment out to remove messages
  
  //Set final state detected particles
  //AddParticle(particle,true/false you want to write in final vector, genID for linking to generated truth value)
  //Note if particle is added to final with a valid genID it will be used
  //to determine the correct permutation of the simulated event
  AddParticle(&fElectron,kTRUE,3);
  AddParticle(&fProton,kTRUE,2);
  AddParticle(&fPip,kTRUE,0);
  AddParticle(&fPim,kTRUE,1);

  PrepAddParticle(&fElectron);
  PrepAddParticle(&fProton);
  PrepAddParticle(&fPip);
  PrepAddParticle(&fPim);

  // TODO : PrepAddParcticle(&fElectron)...G

  //Set final state parents
  
  
  
  TString PID("NONE"); //set this to which particles you want to id via pdg code alone, or set it in individual AddTopology
  TString INCLUSIVE("ALL");//set this to which particles you want reaction to be inclusive of, or set it in individual AddTopology "ALL"=> completely inclusive

  //include topology for analysis and get index
  AddTopology("e-:pi+:pi-:proton",
             bind(&THS2pi::Init_Iter0, this),
             bind(&THS2pi::Topo_0, this),
             PID,INCLUSIVE);

  AddTopology("e-:pi+:proton",
             bind(&THS2pi::Init_Iter1, this),
             bind(&THS2pi::Topo_1, this),
             PID,INCLUSIVE);

  AddTopology("e-:pi-:proton",
             bind(&THS2pi::Init_Iter2, this),
             bind(&THS2pi::Topo_2, this),
             PID,INCLUSIVE);

  AddTopology("e-:pi+:pi-",
             bind(&THS2pi::Init_Iter3, this),
             bind(&THS2pi::Topo_3, this),
             PID,INCLUSIVE);

  
  
  THSFinalState::InitFS();
}

void THS2pi::FileStart(){
  //May be called when a new file is opened
  fTrigger.SetParticles(frDetParts); //the detected particles
  fTrigger.SetEventInfo(fEventInfo);//once per event info
  fTrigger.SetRunInfo(fRunInfo);//once per run info

  //Configure the cuts (default are delta timing cuts in ns)
  fCuts.SetElCut(2,2); //Just FT,FD
  fCuts.SetPionPCut(0,2,0,2);//FT,FDTOF,CD,FDCAL
  fCuts.SetPionMCut(0,2,0,2);//FT,FDTOF,CD,FDCAL
  fCuts.SetProtCut(0,2,1,2);//FT,FDTOF,CD,FDCAL
  fCuts.SetGammaCut(2,2,0,2);//FT,FDTOF,CD,FDCAL

  //Initilaise THSMVA stuff
  // add config stuff here
  std::cout<<"Setting MVA trees..."<<std::endl;
  fMVAPrep.SetTree(fFinalTree);
  fMVATrain.SetTrainTree(fFinalTree);
  
  
  if(THSFinalState::frGenParts) fTrigger.SetSim();//Should get this from RunInfo but not correct in EB at the moment
  
}

//Define topology Iterator functions
void THS2pi::Init_Iter0(){
  //THSParticle iterator initialisation
  //For topology e-:pi+:pi-:proton

   AutoIter(); //Let finalstate try and work out the iterattor for you, you can remove this if you want to do it yourself
}
void THS2pi::Init_Iter1(){
  //THSParticle iterator initialisation
  //For topology e-:pi+:proton

   AutoIter(); //Let finalstate try and work out the iterattor for you, you can remove this if you want to do it yourself
}
void THS2pi::Init_Iter2(){
  //THSParticle iterator initialisation
  //For topology e-:pi-:proton

   AutoIter(); //Let finalstate try and work out the iterattor for you, you can remove this if you want to do it yourself
}
void THS2pi::Init_Iter3(){
  //THSParticle iterator initialisation
  //For topology e-:pi+:pi-

   AutoIter(); //Let finalstate try and work out the iterattor for you, you can remove this if you want to do it yourself
}
// void THS2pi::Init_IterX(){
//  THSParticleIter* diter=CreateParticleIter(fTID_X,&fVecY,NPARTICLES);
//  ...
// }
//Define topology functions
void THS2pi::Topo_0(){
  //For topology e-:pi+:pi-:proton
  //if(fElectron.Detector()>0) {fGoodEvent=kFALSE;return;} //Put some cuts on particle detectors
  //Define starttime from electron candidate
  if(fElectron.Detector()<-1)fElectron.SetPath(1.97);
  // fPip.SetPath(fPip.Path()*0.96);
  // fPim.SetPath(fPim.Path()*0.98);
  // fProton.SetPath(fProton.Path()*0.95);
  Float_t stime=fTrigger.StartTime(&fElectron);
  fTrigger.SubtractStartTime(&fElectron,&fProton,&fPip,&fPim);

  HSLorentzVector miss=fBeam+fTarget-fElectron.P4()-fProton.P4()-fPip.P4()-fPim.P4();
  fMissMass2=miss.M2();
  fMissMass=miss.M();

 
}
void THS2pi::Topo_1(){
  //For topology e-:pi+:proton
  //if(fElectron.Detector()>0) {fGoodEvent=kFALSE;return;} //Put some cuts on particle detectors
  if(fElectron.Detector()<-1)fElectron.SetPath(1.97);
  Float_t stime=fTrigger.StartTime(&fElectron);
  fTrigger.SubtractStartTime(&fElectron,&fProton,&fPip);
  //Reconstruct missing or combined particles
  HSLorentzVector miss=fBeam+fTarget-fElectron.P4()-fProton.P4()-fPip.P4();
  fMissMass2=miss.M2();
  fMissMass=miss.M();
  fPim.SetP4(miss);
  fPim.TakePDGMass();
 }
void THS2pi::Topo_2(){
  //For topology e-:pi-:proton
  //if(fElectron.Detector()>0) {fGoodEvent=kFALSE;return;} //Put some cuts on particle detectors
  if(fElectron.Detector()<-1)fElectron.SetPath(1.97);
  Float_t stime=fTrigger.StartTime(&fElectron);
  fTrigger.SubtractStartTime(&fElectron,&fProton,&fPim);

  HSLorentzVector miss=fBeam+fTarget-fElectron.P4()-fProton.P4()-fPim.P4();
  fMissMass2=miss.M2();
  fMissMass=miss.M();
  fPip.SetP4(miss);
  fPip.TakePDGMass();
}
void THS2pi::Topo_3(){
  //For topology e-:pi+:pi-
  //if(fElectron.Detector()>0) {fGoodEvent=kFALSE;return;} //Put some cuts on particle detectors
   if(fElectron.Detector()<-1)fElectron.SetPath(1.97);
   Float_t stime=fTrigger.StartTime(&fElectron);
  fTrigger.SubtractStartTime(&fElectron,&fPip,&fPim);
  HSLorentzVector miss=fBeam+fTarget-fElectron.P4()-fPip.P4()-fPim.P4();
  fMissMass2=miss.M2();
  fMissMass=miss.M();
  fProton.SetP4(miss);
  fProton.TakePDGMass();

 
}
// void THS2pi::Topo_X(){
// }

void THS2pi::Kinematics(){
  //configure trigger for this event
  fTrigger.ReadParticles();
  //Do calculations if Good Event
  //if(fIsTMVA) TMVAFill();
  if (fIsTMVA){
      FillVars();
      fMVAPrep.RemoveNaNs();

  }

}
//////////////////////////////////////////////////
/// Define conditions for track to be considered
/// good in event. Adding conditions on junk tracks
///  can greatly reduce combitorials etc.
/// kFALSE=> track ignored completely
Bool_t THS2pi::CheckParticle(THSParticle* part){
  if(part->Detector()<0)return kTRUE; //All FT tracks
  if(part->PDG()==45)return kFALSE; //? what are these?
  if(part->Time()==0)return kFALSE;   //Track needs time
  if(part->Charge()&&part->DeltaE()<2)return kFALSE; //Charged track needs deltaE>2

  return kTRUE;
}

void THS2pi::FinalStateOutTree(TTree* tree){
  fIsTMVA=kFALSE;
  THSFinalState::fFinalTree=tree;
  //tree->Branch("Final",&fFinal);//If you want to save the final THSParticles
  tree->Branch("MissMass",&fMissMass,"MissMass/D");
  tree->Branch("Topo",&fTopoID,"Topo/I");
  tree->Branch("NPerm",&fNPerm,"NPerm/I");
  tree->Branch("NDet",&fNDet,"NDet/I");

}
void THS2pi::TMVAOutTree(TTree* tree){
  THSFinalState::fFinalTree=tree;
  fIsTMVA=kTRUE;
  tree->Branch("MissMass2",&fMissMass2,"MissMass2/D");
  tree->Branch("MissMass",&fMissMass,"MissMass/D");
  tree->Branch("Topo",&fTopoID,"Topo/I");
  tree->Branch("NPerm",&fNPerm,"NPerm/I");
  tree->Branch("NDet",&fNDet,"NDet/I");
  //tree->Branch("Detector",&fDetector,"Detector/I");
  tree->Branch("Correct",&fCorrect,"Correct/I");

  fMVAPrep.SetTree(tree);
  fMVAPrep.SetBranches();

  fMVATrain.SetTrainTree(tree);

  tree->GetDirectory()->cd();
  fMVAPrep.SetName("Test");
  fMVAPrep.Write();

}
void THS2pi::TMVAFill(){
  fElTime=fElectron.DeltaTime();
  fElEdep=fElectron.Edep();
  fElDeltaE=fElectron.DeltaE();
  fElPreE=fElectron.PreE();
  fElP=fElectron.P4p()->P();
  fElTh=fElectron.P4p()->Theta();
  fElPhi=fElectron.P4p()->Phi();
  fElVz=fElectron.Vertex().Z();
  fElTrChi2=fElectron.TrChi2();
  fElDet=fCuts.Detector(fElectron.Detector());
 
  fPTime=fProton.DeltaTime();
  fPEdep=fProton.Edep();
  fPDeltaE=fProton.DeltaE();
  fPPreE=fProton.PreE();
  fPP=fProton.P4p()->P();
  fPTh=fProton.P4p()->Theta();
  fPPhi=fProton.P4p()->Phi();
  fPVz=fProton.Vertex().Z();
  fPTrChi2=fProton.TrChi2();
  fPDet=fCuts.Detector(fProton.Detector());

  fPipTime=fPip.DeltaTime();
  fPipEdep=fPip.Edep();
  fPipDeltaE=fPip.DeltaE();
  fPipPreE=fPip.PreE();
  fPipP=fPip.P4p()->P();
  fPipTh=fPip.P4p()->Theta();
  fPipPhi=fPip.P4p()->Phi();
  fPipVz=fPip.Vertex().Z();
  fPipTrChi2=fPip.TrChi2();
  fPipDet=fCuts.Detector(fPip.Detector());

  fPimTime=fPim.DeltaTime();
  fPimEdep=fPim.Edep();
  fPimDeltaE=fPim.DeltaE();
  fPimPreE=fPim.PreE();
  fPimP=fPim.P4p()->P();
  fPimTh=fPim.P4p()->Theta();
  fPimPhi=fPim.P4p()->Phi();
  fPimVz=fPim.Vertex().Z();
  fPimTrChi2=fPim.TrChi2();
  fPimDet=fCuts.Detector(fPip.Detector());
}

/**
 * Add particles to vector
 *
 */

void THS2pi::PrepAddParticle(THSParticle *part){
    fParticles.push_back(part);
}


/**
 * Fill variable vector
 *
 */

void THS2pi::FillVars(){

    fNParts = fParticles.size();

    for (UInt_t i = 0; i<fNParts; i++){
        fMVAPrep.AddVarsFromParticle(fParticles[i], i);
    }
}

