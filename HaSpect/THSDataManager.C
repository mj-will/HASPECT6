#include "THSDataManager.h"
#include "TSystem.h"
#include "TObjArray.h"


THSDataManager::~THSDataManager(){
  if(fTGenerated) delete fTGenerated;
  if(fTUID) delete fTUID;


  for(UInt_t i=0;i<fParticles.size();i++){
    if(fParticles[i])delete fParticles[i];  
  }
  for(UInt_t i=0;i<fGenerated.size();i++){
    if(fGenerated[i])delete fGenerated[i];  
  }
  
  fParticles.clear();
  fGenerated.clear();

  
  if(fReadParticles){
    for(UInt_t i=0;i<fReadParticles->size();i++){
      if(fReadParticles->at(i)) delete fReadParticles->at(i);  
    }
  fReadParticles->clear();
  }
  if(fReadGenerated){
    for(UInt_t i=0;i<fReadGenerated->size();i++){
      if(fReadGenerated->at(i)) delete fReadGenerated->at(i);  
    }
  fReadGenerated->clear();
  }

  CloseReadTree();
  
  // if(fReadParticles) delete fReadParticles;
  if(fWeights)delete fWeights;
}
void THSDataManager::CloseReadTree(){
  if(fReadFile) {
    //if(fReadTree) delete fReadTree;
    fReadFile->Close();
    delete fReadFile;
  }

}
Bool_t THSDataManager::InitTreeReader(TString filename,TString name){
  fReadFile=TFile::Open(filename);
  if(!fReadFile) {Error("THSDataManager::Init","No file found %s",filename.Data());return kFALSE;}
  fReadTree=dynamic_cast<TTree*>(fReadFile->Get(name));
  if(!fReadTree){Error("THSDataManager::Init","No tree found %s",name.Data());return kFALSE;}
  //standard particle vector
  fTParticles= TTreeReaderArray<THSParticle>{fTReader, fReadBName.Data()};
  //generated particles if simualtions
  if(fInGenerated)fTGenerated= new TTreeReaderArray<THSParticle>{fTReader, fReadGName.Data()};
  if(fReadTree->GetBranch(fIDName)) fTUID=new TTreeReaderValue<Double_t>{fTReader, fIDName};
  
  fTReader.SetTree(fReadTree);
  return kTRUE;
}
Bool_t THSDataManager::Init(TString filename,TString name){
  return InitTreeReader(filename,name); //default use tree reader

}
Bool_t THSDataManager::ReadEvent(Long64_t entry){
  // if(fReadParticles){
  //   if(entry<0){
  //     if(fReadTree->GetEntry(fEntry++)>0){
  // 	for(UInt_t iv=0;iv<fReadParticles->size();iv++)
  // 	  fParticles[iv]=fReadParticles->at(iv);      
  // 	return kTRUE;
  //     }
  //     else return kFALSE;
  //   }
  //   if(fReadTree->GetEntry((fEntry=entry))>0){
  //     for(UInt_t iv=0;iv<fReadParticles->size();iv++)
  // 	fParticles[iv]=fReadParticles->at(iv);
  //     return kTRUE;
  //   }
  //   else return kFALSE;
  // }
  //OR using TTreeReader for project
  //else if(fTReader.GetTree()){
  Bool_t IsNext=kFALSE;
  if(fTReader.GetTree()){
    if(entry<0){
      fEntry++;
      IsNext= fTReader.Next();
    }
    else
      IsNext= !(fTReader.SetLocalEntry(fEntry=entry));
    
    ClearParticles();
    for(UInt_t ip=0;ip<fTParticles.GetSize();ip++){
      fParticles.push_back((THSParticle*)&(fTParticles.At(ip)));
    }
    if(fInGenerated){
      for(UInt_t ip=0;ip<fTGenerated->GetSize();ip++){
    	fGenerated.push_back((THSParticle*)&(fTGenerated->At(ip)));
      }
    }
    return IsNext;
  }
  Info("THSDataManager::ReadEvent"," No tree initialised");
  return kFALSE;
}
void THSDataManager::ReadWriteChain(TChain* chain,TString OutDirName,TString FileAppend){
  //loop over all files in chain
  TObjArray *files=chain->GetListOfFiles();

  Info("THSDataManager::ReadWriteChain"," Will proceess all files in chain and put in %s with %s appended",OutDirName.Data(),FileAppend.Data());
  
  for(Int_t i=0;i<files->GetEntries();i++){
    TString fname=files->At(i)->GetTitle();
    Init(fname,chain->GetName());
    TString outfile=gSystem->BaseName(fname);
    outfile.ReplaceAll(fname(fname.Last('.'),fname.Sizeof()),FileAppend);
    Info("THSDataManager::ReadWriteChain"," Writing to %s",(OutDirName+outfile).Data());
    WriteParticles(OutDirName+"/"+outfile);
    CloseReadTree();
  }
}

void THSDataManager::WriteParticles(TString filename){
  //Write all input events on 1 go
  InitOutput(filename);
  while(ReadEvent()){
    fWriteTree->Fill();
  }
  CloseOutput();
}
void THSDataManager::InitOutput(TString filename){
  fWriteFile=new TFile(filename,"recreate");
  fWriteTree=new TTree("HSParticles","data tree");
  //fReadParticles=&fParticles;
  fWriteTree->Branch(fReadBName,&fParticles);
  if(fAddGenerated)fWriteTree->Branch(fWriteGName,&fGenerated);
  Info("THSDataManager::InitOutput","Saving %d particles",(Int_t)fParticles.size());
}
void THSDataManager::CloseOutput(){
  fWriteFile->cd();
  fWriteTree->Write();
  fWriteFile->Close();
  delete fWriteFile;
  
}
void THSDataManager::PrintEvent(Long64_t entry){

  Info("PrintLundEvent","Printing First event, this should help configure project class");
  if(!ReadEvent(entry)) {Error("PrintEvent","error in event read");return;}  
  cout<<"Initialised with "<<fNin<<" particles"<<endl;
  for(UInt_t i=0;i<fNin;i++){
    fParticles[i]->Print("");  
  }
  
}
void THSDataManager::AddUID(TString filename,TString treename){
  //Once all files are available you can give every
  //event a unique ID for use in synchronisating weights etc.
  
  TFile* file=TFile::Open(filename,"update");
  if(!file) {Error("THSDataManager::AddUID","No file %s found",filename.Data());return;}
  TTree* tree=(TTree*)file->Get(treename);
  if(!tree) {Error("THSDataManager::AddUID","No tree %s found in %s",treename.Data(),filename.Data());return;}
  if(tree->GetBranch("UID")) return; //aready exists
  TBranch* branch=tree->Branch("UID",&UID,"UID/D");
  if(!branch) return;
  
  Long64_t id=(Long64_t)UID;
  UID=id;
   for(Long64_t i=0;i<tree->GetEntries();i++){
    branch->Fill();
    id++;
    UID=id;   
   }
   Info("THSDataManager::AddUID","Added UID for file %s ",filename.Data());
   tree->Write(tree->GetName(),TObject::kOverwrite);
   file->Close();
   delete file;
   
}
void THSDataManager::LoadWeights(TString fname,TString wname){
    Info("LoadWeights"," Opening weights file %s name %s",fname.Data(),wname.Data());
    fWeights=new THSWeights();
    fWeights->LoadSaved(fname,wname);
    fWeights->PrintWeight();
   
}
void THSDataManager::GetWeightEvent(){
  if(fWeights)fWeights->GetEntryBinarySearch((Long64_t)fTUID);
}
Double_t THSDataManager::GetWeight(Int_t species){
  if(fWeights) fWeight=fWeights->GetWeight(species);
  return fWeight;
}
