#include <sstream>
#include <string>
#include "THSHipoReader.h"

THSHipoReader::THSHipoReader(){
  fHipo = new THipo();
  fHipo->ConfigBank("REC::Particle");
  //fHipo->ConfigBank("REC::Detector");
  fHipo->ConfigBank("FT::particles");
  //Get the necessary items from Particle Bank
  fPBank=fHipo->GetBank("REC::Particle");
  fPx=fPBank->GetItem("px");
  fPy=fPBank->GetItem("py");
  fPz=fPBank->GetItem("pz");
  fVx=fPBank->GetItem("vx");
  fVy=fPBank->GetItem("vy");
  fVz=fPBank->GetItem("vz");
  fPid=fPBank->GetItem("pid");
  fMass=fPBank->GetItem("mass");
  fCharge=fPBank->GetItem("charge");
  //Get the necessary items from FT Bank
  fFTBank=fHipo->GetBank("FT::particles");
  fFTPx=fFTBank->GetItem("cx");
  fFTPy=fFTBank->GetItem("cy");
  fFTPz=fFTBank->GetItem("cz");
  fFTPid=fFTBank->GetItem("id");
  fFTTime=fFTBank->GetItem("time");
  fFTEnergy=fFTBank->GetItem("energy");
  fFTCharge=fFTBank->GetItem("charge");

  //Make vectors to hold pointers to THSParticles
  //These are just used for speed, new THSParticles
  //will be added when required by that event.
  //In the end this vector will contain max number of particles
  //from all the events
  fReadParticles=new vector<THSParticle*>;
  
 
}
Bool_t THSHipoReader::Init(TString filename,TString name){
  if(fAddGenerated&&!fMCBank){
    fHipo->ConfigBank("MC::Particle");
    fMCBank=fHipo->GetBank("MC::Particle");
    fMCPx=fMCBank->GetItem("px");
    fMCPy=fMCBank->GetItem("py");
    fMCPz=fMCBank->GetItem("pz");
    fMCVx=fMCBank->GetItem("vx");
    fMCVy=fMCBank->GetItem("vy");
    fMCVz=fMCBank->GetItem("vz");
    fMCPid=fMCBank->GetItem("pid");
    fReadGenerated=new vector<THSParticle*>;
  }
  fHipo->InitFile(filename);
  Info("THSHipoReader","Opened file");
  return kTRUE;
}
Bool_t THSHipoReader::ReadEvent(Long64_t entry){
  //return false at end of file
  // LUND format:
// Header (Event Info):
// # of Particles, # of e, particle id, parent, daughter, p_x, p_y, p_z, E, mass, x vertex, y vertex, z vertex
// type is 1 for particles in the detector
  if(!fHipo->NextEvent()) return kFALSE;
  fEntry++;
  
  fParticles.clear();//reset fParticles
  if(fAddGenerated) fGenerated.clear();//reset fGenerated
  
  if(fAddGenerated&&fMCBank){//First read generated event
    UInt_t NGen=fMCPid->Size();
    while(NGen>fReadGenerated->size()){//make sure enough space in vector
      fReadGenerated->push_back(new THSParticle());
    }
    Int_t ip=0;
    while(fMCBank->NextEntry()){
      THSParticle* particle=fReadGenerated->at(ip++);
      fGenerated.push_back(particle);
      particle->SetXYZT(fMCPx->Val(),fMCPy->Val(),fMCPz->Val(),0);
      particle->SetVertex(fMCVx->Val(),fMCVy->Val(),fMCVz->Val());
      particle->SetPDGcode(fMCPid->Val());
      particle->TakePDGMass();
    }

  }
  //eventbuilder particle bank
  if(fPBank){ //Filling reconstructed fParticles
    UInt_t Nin=fPid->Size();
    //in case some events have more particles
    while(Nin>fReadParticles->size()){
      fReadParticles->push_back(new THSParticle());
    }
    Int_t ip=0;
    while(fPBank->NextEntry()){
       THSParticle* particle=fReadParticles->at(ip++);
      fParticles.push_back(particle);
      particle->SetXYZT(fPx->Val(),fPy->Val(),fPz->Val(),0);
      particle->SetVertex(fVx->Val(),fVy->Val(),fVz->Val());
      particle->SetPDGcode(fPid->ValI());
      particle->TakePDGMass();
      particle->SetMeasMass(fMass->Val());
      if(!fPid->Val()) particle->SetPDGcode(fCharge->ValI()*1E6); //unknown +ve
      particle->SetDetector(100);
     }
  }
  //FT particle bank
  if(fFTBank){ //Filling FT fParticles
    UInt_t Nin=fFTPid->Size()+fParticles.size();
    //in case some events have more particles
    while(Nin>fReadParticles->size()){
      fReadParticles->push_back(new THSParticle());
    }
    Int_t ip=fParticles.size();
    while(fFTBank->NextEntry()){
      THSParticle* particle=fReadParticles->at(ip++);
      fParticles.push_back(particle);
      particle->SetXYZT(fFTPx->Val(),fFTPy->Val(),fFTPz->Val(),fFTEnergy->Val());
      //  particle->SetPDGcode(fFTPid->Val());
      particle->SetPDGcode(0);
      particle->SetPDGcode(fFTCharge->ValI()*1E6); //unknown PID
      particle->SetDetector(300);
      particle->SetTime(fFTTime->Val());
     }
  }

  
  return kTRUE;
  
}
