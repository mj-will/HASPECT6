#include <TROOT.h>
#include <TDatabasePDG.h>
#include "RootBeerUtil.h"	//gets everything for RootBeer scheme
#include "TDST.h"
#include "THSCLASg13.h"
#include "particleDEF.h"

#include "g13/decayutils/DecayTrackCorrections.h"
#include "g13/decayutils/DecayFiducialCuts.h"
//#include "DecayTrackCorrections.h"
//#include "DecayFiducialCuts.h"
#include "extra_packages/eloss_rb/eLoss.h"              //For energy loss corrections
#include "g13/LinearPol.h"
#define PARA 0                  //defs for polarisation plane
#define PERP 1
#define AMO  2
#define CHANGING 3
#define ALUMINUM 0               //defs for radiators
#define AMORPHOUS 1
#define DIAMOND 2
#define UNKNOWN 3

// //for PM momentum and enrgy loss corrections

THSCLASg13::THSCLASg13() : THSRootBeer("EPIC:EVNT:ECPB:SCPB:CCPB:TAGR:TGBI:DCPB:STPB:MCTK:MCVX") {
  //for PM momentum and enrgy loss corrections
  
  fEventInfo=new THSEventInfo();
}

Bool_t THSCLASg13::Init(TString filename,TString name){
  THSRootBeer::Init(filename,name);
  //Load fake MC run number
  if(fAddGenerated){
    fRun_number = TString("054044").Atoi();
    // LoadMCCor("/home/dglazier/Dropbox/g13Sim/MCCor/totalSept16.root");
  }
  //Paul Mattione Decay Utils setup
  Create_dtcCorrectionFunctions(fRun_number); //Creates the correction functions, 		only need to call this once.
  Set_dfcTrackDirectionCuts(fRun_number);
  //initialise eloss
  initELoss(0.,0.,-20.,-24.06);
  
  //Generated Particles
  // if(fAddGenerated)fReadGenerated=new vector<THSParticle>;
  //Linear Pol
  SortPolarisations(filename);
  
  
  return kTRUE;
}
Bool_t THSCLASg13::ReadEvent(Long64_t entry){
  if(!RBEvent()) return kFALSE;

  if(EPIC_NS){
    EPICSEvent();
    return kTRUE;//not saving this event just getting epics info	
  }
  if(EVNT_NH>0){
    Double_t vl = 29.9792458;
    Double_t  starttime=SCPB[EVNT[0].SCstat-1].Time-(SCPB[EVNT[0].SCstat-1].Path)/vl/EVNT[0].Betta;//OR THIS COULD BE FROM THE HEVT STT
    if(!fIsSim)fEventInfo->SetCJStartTime(starttime);
    //make particles for this event
    MakeDetected();
    //get tagged photons for this event
    MakeBeam(-20,200);
    //Any mc tracks
    if(fAddGenerated)MakeTruth();
  }
  return kTRUE;
  
}
 void THSCLASg13::MakeDetected(){
  
   for(Int_t ip=0;ip<EVNT_NH;ip++){
     if((EVNT[ip].DCstat == 0)||((EVNT[ip].ECstat == 0)&&(EVNT[ip].SCstat == 0))) { continue;}
     if(EVNT[ip].Status == 0) { continue;}
     //recalcualte the mass2 for this event
     if(EVNT[ip].Betta==0) continue;//need to chack effect of this

     MakeParticle(ip);
  }
 
}
void THSCLASg13::MakeParticle(Int_t ip){
 
  THSParticle hsp;
  hsp.SetPDGcode(EVNT[ip].ID );
  hsp.SetVertex(EVNT[ip].X,EVNT[ip].Y,EVNT[ip].Z);
  //set the intitial Lorentz Vector
  hsp.SetXYZM(EVNT[ip].Pmom*EVNT[ip].Cx,
		 EVNT[ip].Pmom*EVNT[ip].cy,
		 EVNT[ip].Pmom*EVNT[ip].cz,0);
  //set the measured mass
  hsp.SetMeasMass(sqrt((1/EVNT[ip].Betta/EVNT[ip].Betta-1)*EVNT[ip].Pmom*EVNT[ip].Pmom));

  if(EVNT[ip].SCstat-1 >=0 ){
    hsp.SetTime(SCPB[EVNT[ip].SCstat-1].Time);//-0.667
    hsp.SetPath(SCPB[EVNT[ip].SCstat-1].Path/100);
    hsp.SetDeltaE(SCPB[EVNT[ip].SCstat-1].Edep);
  }
  if(EVNT[ip].ECstat-1 >=0 ){
    hsp.SetEdep(ECPB[EVNT[ip].ECstat-1].Etot);
    hsp.SetPreE(ECPB[EVNT[ip].ECstat-1].Ein);
    if(EVNT[ip].SCstat-1 < 0){
      hsp.SetTime(ECPB[EVNT[ip].ECstat-1].Time);//-0.667
      hsp.SetPath(ECPB[EVNT[ip].ECstat-1].Path/100);
    }
  }
  if(EVNT[ip].CCstat-1 >=0 ){
   hsp.SetNPhot(CCPB[EVNT[ip].CCstat-1].Nphe);
   //   cout<<CCPB[EVNT[ip].CCstat-1].Nphe<<" "<<endl;
  } 
//DeltaT for different particle hypothesis
  Int_t sector = floor(double(DCPB[EVNT[ip].DCstat-1].ScTr) / 100.);  //from daria;s Particle.h
  TLorentzVector P4hyp0=hsp.GetTLorentzVector();
  TLorentzVector P4hyp=hsp.GetTLorentzVector();
    // reference run number, PDG ID, sector of particle, four-vector
  if(EVNT[ip].Charge){
    P4hyp.SetRho(Correct_Momentum(fRun_number,EVNT[ip].Charge * 211, sector, P4hyp0));
    hsp.SetHypPPi(eLoss(&P4hyp,PI_CHARGED_MASS,hsp.GetVertex3(),ELOSS_TARG_LD2,ELOSS_CELL_g13b)->P());
    //Corrected momentum if proton
    P4hyp.SetRho(Correct_Momentum(fRun_number,EVNT[ip].Charge * 2212, sector, P4hyp0));
    hsp.SetHypPNuc(eLoss(&P4hyp,PROTON_MASS,hsp.GetVertex3(),ELOSS_TARG_LD2,ELOSS_CELL_g13b)->P());
    //Corrected momentum if kaon
    P4hyp.SetRho(Correct_Momentum(fRun_number,EVNT[ip].Charge * 321, sector, P4hyp0));
    hsp.SetHypPK(eLoss(&P4hyp,KAON_CHARGED_MASS,hsp.GetVertex3(),ELOSS_TARG_LD2,ELOSS_CELL_g13b)->P());
    //Corrected momentum if electron	
    P4hyp.SetRho(Correct_Momentum(fRun_number,EVNT[ip].Charge * -11, sector, P4hyp0));
    hsp.SetHypPElGam(eLoss(&P4hyp,ELECTRON_MASS,hsp.GetVertex3(),ELOSS_TARG_LD2,ELOSS_CELL_g13b)->P());
  }
  else{ //neutron or gamma
 

  }

  // Pauls' fiducial function:
  // Takes as options: run numbern (int), charge (double), sector (int), vertex (double), four-momentum (const TLorentzVector), returns boolean
  hsp.SetFidCut(Cut_dfcTrackDirection(fRun_number, 1., sector, EVNT[ip].Z, (const TLorentzVector)P4hyp0));

  hsp.SetDetector(sector);
  fParticles.push_back(hsp);
 
}
Bool_t THSCLASg13::MakeBeam(Float_t Tmid,Float_t Tcut){
  //Find a photon within a Tcut window of Tmid  
   if(!TAGR_NH) return kFALSE;
  
  Double_t Egamma=0;
  THSParticle* fHSgamma=0;
  for(Int_t im=0;im<TAGR_NH;im++) {
  if((TAGR[im].TPHO)<40&&(TAGR[im].TPHO)>0){
      if((TAGR[im].STAT!=15)&&(TAGR[im].STAT!=7)) continue; //check good photon 	
      THSParticle hsp(-22);
      Egamma=TAGR[im].ERG;
      hsp.SetXYZT(0,0,Egamma,Egamma);
      hsp.SetTime(TAGR[im].TPHO);//TPHO=TRF+zcentre/c
      hsp.SetPath(0); //already at centre
      hsp.SetDetector(-1);
      //LinPol
      if (fCurrentPlane == 0) hsp.SetVertex(GetLinPol(0, fCurrentEdge, Egamma*1000., 8, 0.2, 0.3),0,0);
      else if (fCurrentPlane == 1) hsp.SetVertex(0,GetLinPol(1, fCurrentEdge, Egamma*1000., 8, 0.2, 0.3),0);
      // else  fHSgamma->SetVertex(0,0,0);
      fParticles.push_back(hsp);
    }
  }
 
  return kTRUE; //got one good photon
}
void THSCLASg13::MakeTruth(){
   for(Int_t ip=0;ip<MCTK_NH[0];ip++){//sort pi-
    MakeTruthParticle(ip);
  } 
}
void THSCLASg13::MakeTruthParticle(Int_t ip){
 
  THSParticle hsp;	
  hsp.SetPDGcode(MCTK[0][ip].id);
  hsp.SetXYZM(MCTK[0][ip].cx*MCTK[0][ip].pmom,MCTK[0][ip].cy*MCTK[0][ip].pmom,MCTK[0][ip].cz*MCTK[0][ip].pmom,MCTK[0][ip].mass);
  hsp.SetVertex(MCVX[ip].x,MCVX[ip].y,MCVX[ip].z);
  fGenerated.push_back(hsp);
}
Int_t THSCLASg13::EPICSEvent(){
  //int nEpicEvent=0;               //Counter for EPICs events
  //  int current_plane=0;            //Stor value of coherent plane
  float plane;
  float epics;
 				         //for getting epics values
  Int_t currentRadiator=UNKNOWN;    	// current rad, plane - init these to UNKNOWN
  fCurrentPlane=UNKNOWN;

  // getEpics(TString("coh_plane").Data(),&plane);
   getEpics("coh_plane",&plane);
  
  getEpics("coh_radiator", &epics);	    	     //Get the radiator
  currentRadiator=(int)epics;	
  //cout<<"epics rad "<<currentRadiator<<endl;
  if((int)currentRadiator==AMORPHOUS){	     //If amorph, set plane to amo
    fCurrentPlane=AMO;
    //LinPol->SetXYZ(0,0,0);
    return 0;
  }
  else if(currentRadiator==DIAMOND){	   	//If diamond, read plane from epics
    getEpics("coh_plane",&epics);
    fCurrentPlane=(int)epics;
    getEpics("coh_edge",&fCurrentEdge);
    //  cout<<"epics diamond "<<fCurrentEdge<<" "<<fCurrentPlane<<endl;
    return 0;
  }	
  
  else return -1;
}	
void THSCLASg13::LoadMCCor(TString corname){
  cout<<"Loading MCCor from "<<corname<<endl;
  TDirectory* savedir=gDirectory;
  TFile* cfile=new TFile(corname);
  if(!cfile->IsOpen()){ cout<<"No MCCor file found!"<<endl; return;}
  savedir->cd();
  fMCCor[2212]=(TH1F*)cfile->Get("ProtonCor")->Clone();
  fMCCor[211] =(TH1F*)cfile->Get("PiPlusCor")->Clone();
  fMCCor[-211]=(TH1F*)cfile->Get("PiMinusCor")->Clone();
  cfile->Close();
  delete cfile;
}
Double_t THSCLASg13::Correct_Momentum(Int_t locRunNumber,Int_t locID,Int_t locSector, const TLorentzVector &locP4){
  if(fAddGenerated)
    return Correct_MCTrackMomentum(locID,locP4);
  else
    return Correct_dtcTrackMomentum(locRunNumber,locID,locSector, locP4);
}
Double_t THSCLASg13::Correct_MCTrackMomentum(Int_t pdg,TLorentzVector p4){
  Double_t Pmom=p4.Rho();
  
  if(pdg!=2212||pdg!=211||pdg!=11) return Pmom;//no corrections for K
 
  return (fMCCor[pdg]->GetBinContent(fMCCor[pdg]->FindFixBin(p4.Theta(),Pmom)))*Pmom+Pmom;
}
