#include <sstream>
#include <string>
#include "THSRADSReader.h"

Bool_t THSRADSReader::Init(TString filename,TString name){
  if(filename.Length()) fTxtIn.open(filename.Data());
  if(!fTxtIn.is_open()) {Error("InitLundEvent","file not found %s",filename.Data());return kFALSE;}
  //Get first event header for Nparticles.
  //We will assume for now all events have same final state
  fTxtIn>>fNin;
  fReadParticles=new vector<THSParticle*>;
  fReadGenerated=new vector<THSParticle*>;
  for(UInt_t i=0;i<fNin;i++){//create lorentzvector for each 
    fReadParticles->push_back(new THSParticle());
    fReadGenerated->push_back(new THSParticle());
  }
  Info("InitLundEvent","Opened file with %d particles",fNin);
  fTxtIn.close();
  fTxtIn.open(filename.Data());//move back to start
  return kTRUE;

}
Bool_t THSRADSReader::ReadEvent(Long64_t entry){
  //return false at end of file
  // LUND format:
// Header (Event Info):
// # of Particles, # of e, particle id, parent, daughter, p_x, p_y, p_z, E, mass, x vertex, y vertex, z vertex
// type is 1 for particles in the detector

  fParticles.clear();//reset fParticles
  if(fAddGenerated) fGenerated.clear();//reset fGenerated

  Double_t dummy;
  Double_t RUNN,EVN,Weight;
  string sline;
  getline(fTxtIn,sline);
  if(fTxtIn.eof()) return kFALSE;
  stringstream streamline(sline);
  streamline>>fNin>>RUNN>>EVN>>dummy>>dummy>>dummy>>dummy>>dummy>>Weight;
  //now read in each particle ans assign to fParticles
  Int_t index,type,pid,parent,daughter;
  Float_t charge,px,py,pz,energy,mass,vx,vy,vz,measmass,time;
  stringstream streamlinep;
  if(fAddGenerated&&RUNN==0){//First read generated event
    while(fNin>fReadGenerated->size()){
      fReadGenerated->push_back(new THSParticle());
    }
    for(UInt_t i=0;i<fNin;i++){
      fGenerated.push_back(fReadGenerated->at(i));
      getline(fTxtIn,sline);
      streamlinep.str(sline);//set sline as string in stream 
      streamlinep>>index>>charge>>type>>pid>>parent>>daughter>>px>>py>>pz>>energy>>mass>>vx>>vy>>vz>>measmass>>time; //get values from stream
      fGenerated[i]->SetXYZT(px,py,pz,energy);
      fGenerated[i]->SetVertex(vx,vy,vz);
      fGenerated[i]->SetPDGcode(pid);
      fGenerated[i]->SetTime(time);
      fGenerated[i]->SetMeasMass(measmass);
      streamlinep.clear();
    }

    //Done generated, now get header for reconstructed
    getline(fTxtIn,sline);
    if(fTxtIn.eof()) return kFALSE;
    stringstream streamline(sline);
    streamline>>fNin>>RUNN>>EVN>>dummy>>dummy>>dummy>>dummy>>dummy>>Weight;
    
  }
  else{
    if(fAddGenerated)
      Error("THSRADSReader::ReadEvent","Trying to read file without generated events i.e. Should have Header RUNN=0");
    else Error("THSRADSReader::ReadEvent","Need to set a generated branch name SetGenBranch(name)");
  }

  if(RUNN){ //Filling reconstructed fParticles
  //in case some events have more particles
    while(fNin>fReadParticles->size()){
      fReadParticles->push_back(new THSParticle());
    }
    for(UInt_t i=0;i<fNin;i++){
      fParticles.push_back(fReadParticles->at(i));
      getline(fTxtIn,sline);
      streamlinep.str(sline);//set sline as string in stream 
      streamlinep>>index>>charge>>type>>pid>>parent>>daughter>>px>>py>>pz>>energy>>mass>>vx>>vy>>vz>>measmass>>time; //get values from stream
      fParticles[i]->SetXYZT(px,py,pz,energy);
      fParticles[i]->SetVertex(vx,vy,vz);
      fParticles[i]->SetTime(time);
      fParticles[i]->SetMeasMass(measmass);

      if(!pid) fParticles[i]->SetPDGcode(charge*1E6); //unknown +ve
      else fParticles[i]->SetPDGcode(pid);
      cout<<i<<" "<<index<<" "<<fParticles[i]->PDG()<<endl;

      streamlinep.clear();
    }
  }
  return kTRUE;
  
}
