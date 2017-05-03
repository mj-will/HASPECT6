#include "THSDataReader.h"


THSDataReader::~THSDataReader(){
  for(UInt_t i=0;i<fParticles.size();i++){
    delete fParticles[i];  
  }
  
  fParticles.clear();

  if(fReadFile) {
    if(fReadTree) delete fReadTree;
    fReadFile->Close();
    delete fReadFile;
  }
  // if(fReadParticles) delete fReadParticles;
}
Bool_t THSDataReader::Init(TString filename,TString name){
  fReadFile=TFile::Open(filename);
  if(!fReadFile) {Error("THSDataReader::Init","No file found %s",filename.Data());return kFALSE;}
  fReadTree=dynamic_cast<TTree*>(fReadFile->Get(name));
  if(!fReadTree){Error("THSDataReader::Init","No tree found %s",name.Data());return kFALSE;}
  fReadParticles=new vector<THSParticle*>;//note crashes on GetEntry without this...
  fReadTree->SetBranchAddress(fReadBName,&fReadParticles);
  fReadTree->GetEntry(0);
  fNin=fReadParticles->size();
  for(Int_t i=0;i<fNin;i++)
    fParticles.push_back(fReadParticles->at(i));
  Info("THSDataReader::Init","Found %d particles in first event",fNin);
  return kTRUE;
}
Bool_t THSDataReader::ReadEvent(Long64_t entry){
  
  if(entry<0){
    if(fReadTree->GetEntry(fEntry++)>0){
      for(UInt_t iv=0;iv<fReadParticles->size();iv++)
	fParticles[iv]=fReadParticles->at(iv);      
      return kTRUE;
    }
    else return kFALSE;
  }
  if(fReadTree->GetEntry((fEntry=entry))>0){
  for(UInt_t iv=0;iv<fReadParticles->size();iv++)
    fParticles[iv]=fReadParticles->at(iv);
    return kTRUE;
  }
  else return kFALSE;
}
void THSDataReader::WriteParticles(TString filename){
  //Write all input events on 1 go
  //Open file for writing
  // fWriteFile=new TFile(filename,"recreate");
  // fWriteTree=new TTree("HSParticles","data tree");
  // fWriteTree->Branch(fReadBName,&fParticles);
  InitOutput(filename);
  while(ReadEvent()){
    fWriteTree->Fill();
  }
  // fWriteFile->cd();
  // fWriteTree->Write();
  // fWriteFile->Close();
  // delete fWriteFile;
  CloseOutput();
}
void THSDataReader::InitOutput(TString filename){
  fWriteFile=new TFile(filename,"recreate");
  fWriteTree=new TTree("HSParticles","data tree");
  //fReadParticles=&fParticles;
  fWriteTree->Branch(fReadBName,&fParticles);
  Info("THSDataReader::InitOutput","Saving %d particles",(Int_t)fParticles.size());
}
void THSDataReader::CloseOutput(){
  fWriteFile->cd();
  fWriteTree->Write();
  fWriteFile->Close();
  delete fWriteFile;
  
}
void THSDataReader::PrintEvent(Long64_t entry){

  Info("PrintLundEvent","Printing First event, this should help configure project class");
  if(!ReadEvent(entry)) {Error("PrintEvent","error in event read");return;}  
  cout<<"Initialised with "<<fNin<<" particles"<<endl;
  for(Int_t i=0;i<fNin;i++){
    fParticles[i]->Print("");  
  }
  
}
