#include <sstream>
#include "THSTxtTreeReader.h"

Bool_t THSTxtTreeReader::Init(TString filename,TString name){
  if(filename.Length()) fTxtIn.open(filename.Data());
  if(!fTxtIn.is_open()) {Error("THSTxtTreeReader::Init","file not found %s",filename.Data());return kFALSE;}

  //Get Branch information
  string str;
  //Read first line defining branches, should be terminated with END
  while(str!=string("END")) {fTxtIn>>str;if(str!=string("END")){fBNames.push_back(str);fNbranches++;}}

  //Create HSParticles
  //PDG name should be given in branchname as shouldn't change for each event
  for(Int_t i=0;i<fNbranches;i++)
    if(fBNames[i].Contains("THSParticle"))
      {
	Int_t pdg=TDatabasePDG::Instance()->GetParticle(fBNames[i](12,fBNames[i].Sizeof()).String())->PdgCode();
	fParticles.push_back(new THSParticle(pdg));
 	fNin++;
     }
    
  
  return kTRUE;

}
Bool_t THSTxtTreeReader::ReadEvent(Long64_t entry){
  string sline;
  getline(fTxtIn,sline);
  if(fTxtIn.eof()) return kFALSE;
  
  //Loop over branches and read
  Int_t ip=0;//particle counter
  Int_t dInt=0;
  Double_t dDouble=0;
  Float_t dFloat=0;
  Long64_t dLong=0;

  for(Int_t ib=0;ib<fNbranches;ib++){
    if(ib!=0) getline(fTxtIn,sline);
    fStreamline.str(sline);
    
    if(fBNames[ib]==string("THSParticle")){
      ReadHSParticle(fParticles[ip]);
      //Now look to see if truth value given on next line
      if(fBNames[ib+1]==string("HSTruth")) {AddTruth(fParticles[ip]);ib++;}
      ip++;
    }
    //Functions below can be extended and used if needed
    else if(fBNames[ib]==string("I")) dInt=ReadInt();
    else if(fBNames[ib]==string("D")) dDouble=ReadDouble();
    else if(fBNames[ib]==string("F")) dFloat=ReadFloat();
    else if(fBNames[ib]==string("L")) dLong=ReadLong();
    fStreamline.clear();
  }
  return kTRUE;
}

void  THSTxtTreeReader::ReadHSParticle(THSParticle* particle){
  //In txt file THSParticles should contain a line of 8 numbers
  //corresponding to the following variables.
  if(!particle) {Error("THSTxtTreeReader::ReadHSParticle","particle is nullptr");return;}
  //Fill particle with txt branch
  Int_t pdg;
  Double_t px,py,pz,energy;
  Double_t vx,vy,vz,time;
  //read line
  fStreamline>>pdg;
  fStreamline>>px>>py>>pz>>energy;
  fStreamline>>vx>>vy>>vz>>time;

  particle->SetPDGcode(pdg);
  particle->SetXYZT(px,py,px,energy);
  particle->SetMeasMass(particle->P4p()->M());
  particle->TakePDGMass(); //Gives particle PDG mass while keeping momentum fixed
  particle->SetVertex(vx,vy,vz);
  particle->SetTime(time);
}
void  THSTxtTreeReader::AddTruth(THSParticle* particle){
  //In txt file THSParticles should contain a line of 8 numbers
  //corresponding to the following variables.
  if(!particle) {Error("THSTxtTreeReader::ReadHSParticle","particle is nullptr");return;}
  //Fill particle with txt branch
  Int_t pdg;
  Double_t px,py,pz,energy;
  Double_t vx,vy,vz,time;
  //read line
  fStreamline>>pdg;
  fStreamline>>px>>py>>pz>>energy;
  fStreamline>>vx>>vy>>vz>>time;

  particle->SetTruth(TLorentzVector(px,py,pz,energy),TVector3(vx,vy,vz),pdg);
}
Double_t THSTxtTreeReader::ReadDouble(){
  Double_t val;
  fStreamline>>val;
  return val;

}
Float_t THSTxtTreeReader::ReadFloat(){
  Float_t val;
  fStreamline>>val;
  return val;

}
Int_t THSTxtTreeReader::ReadInt(){
  Int_t val;
  fStreamline>>val;
  return val;

}
Long64_t THSTxtTreeReader::ReadLong(){
  Long64_t val;
  fStreamline>>val;
  return val;

}
