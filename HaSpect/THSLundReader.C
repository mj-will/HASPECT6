#include <sstream>
#include <string>
#include "THSLundReader.h"

Bool_t THSLundReader::Init(TString filename,TString name){
  if(filename.Length()) fTxtIn.open(filename.Data());
  if(!fTxtIn.is_open()) {Error("InitLundEvent","file not found %s",filename.Data());return kFALSE;}
  //Get first event header for Nparticles.
  //We will assume for now all events have same final state
  fTxtIn>>fNin;
  fReadParticles=new vector<THSParticle*>;
  for(UInt_t i=0;i<fNin;i++)//create lorentzvector for each 
    fReadParticles->push_back(new THSParticle());
  if(fAddGenerated){
    fReadGenerated=new vector<THSParticle*>;
    for(UInt_t i=0;i<fNin;i++)//create lorentzvector for each 
      fReadGenerated->push_back(new THSParticle());
  }
  Info("InitLundEvent","Opened file with %d particles",fNin);
  fTxtIn.close();
  fTxtIn.open(filename.Data());//move back to start
  return kTRUE;

}
Bool_t THSLundReader::ReadEvent(Long64_t entry){
  if(fAddGenerated) return ReadGenerated(entry); //just fill generated branch
  //return false at end of file
  // LUND format:
// Header (Event Info):
// # of Particles, # of e, particle id, parent, daughter, p_x, p_y, p_z, E, mass, x vertex, y vertex, z vertex
// type is 1 for particles in the detector
  Double_t dummy;
  string sline;
  getline(fTxtIn,sline);
  if(fTxtIn.eof()) return kFALSE;
  stringstream streamline(sline);
  streamline>>fNin>>dummy>>dummy>>dummy>>dummy>>dummy>>dummy>>dummy>>dummy;
  //now read in each particle ans assign to fParticles
   Int_t index,type,pid,parent,daughter;
  Float_t charge,px,py,pz,energy,mass,vx,vy,vz;
  stringstream streamlinep;
  //in case some events have more particles
  while(fNin>fReadParticles->size())fReadParticles->push_back(new THSParticle());
  fParticles.clear();//reset fParticles
  for(UInt_t i=0;i<fNin;i++){
    fParticles.push_back(fReadParticles->at(i));
    getline(fTxtIn,sline);
    streamlinep.str(sline);//set sline as string in stream
   
    streamlinep>>index>>charge>>type>>pid>>parent>>daughter>>px>>py>>pz>>energy>>mass>>vx>>vy>>vz; //get values from stream
    fParticles[i]->SetXYZT(px,py,pz,energy);
    fParticles[i]->SetVertex(vx,vy,vz);
    fParticles[i]->SetPDGcode(pid);
    //Look if additional truth particle given on same line
    streamlinep>>index;
    if(index) {
    	streamlinep>>charge>>type>>pid>>parent>>daughter>>px>>py>>pz>>energy>>mass>>vx>>vy>>vz;
    	fParticles[i]->SetTruth(TLorentzVector(px,py,pz,energy),TVector3(vx,vy,vz),pid);
      }
    streamlinep.clear();
  }
  return kTRUE;
  
}
Bool_t THSLundReader::ReadGenerated(Long64_t entry){
  //return false at end of file
  // LUND format:
// Header (Event Info):
// # of Particles, # of e, particle id, parent, daughter, p_x, p_y, p_z, E, mass, x vertex, y vertex, z vertex
// type is 1 for particles in the detector
  Double_t dummy;
  string sline;
  getline(fTxtIn,sline);
  if(fTxtIn.eof()) return kFALSE;
  stringstream streamline(sline);
  streamline>>fNin>>dummy>>dummy>>dummy>>dummy>>dummy>>dummy>>dummy>>dummy;
  //now read in each particle ans assign to fParticles
   Int_t index,type,pid,parent,daughter;
  Float_t charge,px,py,pz,energy,mass,vx,vy,vz;
  stringstream streamlinep;
  //in case some events have more particles
  while(fNin>fReadGenerated->size())fReadGenerated->push_back(new THSParticle());
  fGenerated.clear();//reset fParticles
  for(UInt_t i=0;i<fNin;i++){
    fGenerated.push_back(fReadGenerated->at(i));
    getline(fTxtIn,sline);
    streamlinep.str(sline);//set sline as string in stream
   
    streamlinep>>index>>charge>>type>>pid>>parent>>daughter>>px>>py>>pz>>energy>>mass>>vx>>vy>>vz; //get values from stream
    fGenerated[i]->SetXYZT(px,py,pz,energy);
    fGenerated[i]->SetVertex(vx,vy,vz);
    fGenerated[i]->SetPDGcode(pid);
    //Look if additional truth particle given on same line
    streamlinep>>index;
    if(index) {
    	streamlinep>>charge>>type>>pid>>parent>>daughter>>px>>py>>pz>>energy>>mass>>vx>>vy>>vz;
    	fGenerated[i]->SetTruth(TLorentzVector(px,py,pz,energy),TVector3(vx,vy,vz),pid);
      }
    streamlinep.clear();
  }
  return kTRUE;
  
}
