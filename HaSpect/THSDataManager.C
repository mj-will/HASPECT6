#include "THSDataManager.h"
#include "TTreeCache.h"
#include "TSystem.h"
#include "TObjArray.h"


THSDataManager::~THSDataManager(){

  // for(UInt_t i=0;i<fParticles.size();i++){
  //   if(fParticles[i])delete fParticles[i];  
  // }
  // for(UInt_t i=0;i<fGenerated.size();i++){
  //   if(fGenerated[i])delete fGenerated[i];  
  // }
  
  fParticles.clear();
  fGenerated.clear();

  
  if(fReadParticles){
   fReadParticles->clear();
  }
  if(fReadGenerated){
   fReadGenerated->clear();
  }

  CloseReadTree();
  // if(fReadParticles) delete fReadParticles;
  if(fWeights)delete fWeights;
  if(fEntryList)delete fEntryList;
}
void THSDataManager::CloseReadTree(){
  if(fReadFile) {
    delete fReadFile;fReadFile=nullptr;
    fReadTree=nullptr;
  }
  if(fPerfstats) delete fPerfstats;fPerfstats=nullptr;

}
Bool_t THSDataManager::InitReader(TString filename,TString name){
  fReadFile=TFile::Open(filename);
  if(!fReadFile) {Error("THSDataManager::Init","No file found %s",filename.Data());return kFALSE;}
  fReadTree=dynamic_cast<TTree*>(fReadFile->Get(name));
  if(!fReadTree){Error("THSDataManager::Init","No tree found %s",name.Data());return kFALSE;}
  //standard particle vector
  fReadParticles=&fParticles;
  fReadTree->SetBranchAddress(fReadBName.Data(),&fReadParticles);
  fReadPIDs=&fPIDs;
  fReadTree->SetBranchAddress("PIDs",&fReadPIDs);
  fBParticles=fReadTree->GetBranch(fReadBName.Data());
  fBPIDs=fReadTree->GetBranch("PIDs");
  cout<<"set brandch "<<fReadParticles<<" "<<&fParticles<<endl;
   //generated particles if simualtions
  fReadGenerated=&fGenerated;
  if(fInGenerated)fReadTree->SetBranchAddress(fReadGName.Data(),&fReadGenerated);
  
  //TTreeCache::SetLearnEntries(100);
  fReadTree->SetCacheSize(50E6);//10MB
  fReadTree->StopCacheLearningPhase();
  fPerfstats = new TTreePerfStats("ioperf", fReadTree);
  
  return kTRUE;
}
Bool_t THSDataManager::Init(TString filename,TString name){
  fTreeName=name;
  fCurFileName=gSystem->BaseName(filename);
  return InitReader(filename,name); //default use tree reader
}
Bool_t THSDataManager::NextChainFile(){
  if(fChainFileN==fChainFiles->GetEntries()){
    cout<<"THSDataManager::NextChainFile() Finished all files "<<endl;
    PrintPerfStats();
    CloseReadTree();
    if(fWriteThis)CloseOutput();
    return kFALSE; //no more files
  }
  CloseReadTree();
  if(fWriteThis)CloseOutput();
  cout<<"THSDataManager::NextChainFile() new file : "<<fChainFiles->At(fChainFileN)->GetTitle()<<" "<<fChainFileN<<" "<<fChainFiles->GetEntries()<<endl;
  //initialise next file
  fEntry=0;
  TString fname=fChainFiles->At(fChainFileN++)->GetTitle();
  Init(fname,fTreeName);
  fCurFileName=gSystem->BaseName(fname);
  if(fOutDir!=TString("")){
    fCurFileName.Append(fFileAppend);
    InitOutput(fOutDir+"/"+fCurFileName);
  }
  if(fEntryList) FilterFinalStateEvent();//If filtering via entrlist, redo for this file
  return kTRUE;
}
Bool_t THSDataManager::ReadEvent(Long64_t entry){
  //Default reader for root files with vector<THSParticle*> branches
  // if(fEntry<fReadTree->GetEntries())fBParticles->GetEntry(fEntry++);
  if(fEntry<fReadTree->GetEntries())fReadTree->GetEntry(fEntry++);
  else if(fChainFiles){
    cout<<fEntry<<" "<<fChainFileN<<" "<<fChainFiles->GetEntries()<<endl;
    if(fChainFileN<=fChainFiles->GetEntries()){//next file
      NextChainFile();  
      //first event in new file

      if(!fReadFile) return kFALSE;
      fReadTree->GetEntry(fEntry++);
    }
  }
  else return kFALSE;
  return kTRUE; 
}
void THSDataManager::FilterFinalStateEvent(Long64_t entry){
  if(fEntryList) delete fEntryList;fEntryList=nullptr;
  fEntryList=new TEntryList();
  //Fill EntryList with events passing finalstate topology
  fReadTree->DropBranchFromCache(fBParticles);
  fReadTree->AddBranchToCache(fBPIDs,kTRUE);
  fReadTree->StopCacheLearningPhase();
  for(Int_t i=0;i<fReadTree->GetEntries();i++){
     fBPIDs->GetEntry(i);
     if(fFinalState->CheckForATopology())
       fEntryList->Enter(i);
  }
  fReadTree->SetEntryList(fEntryList);
  fReadTree->DropBranchFromCache(fBPIDs);
  fReadTree->AddBranchToCache(fBParticles,kTRUE);

  cout<<"THSDataManager::FilterFinalStateEvent found final state entries = "<<fEntryList->GetN()<<endl;
}
Bool_t THSDataManager::ReadFinalStateEvent(Long64_t entry){
  //Default reader for root files with vector<THSParticle*> branches
  if(fEntry<fReadTree->GetEntries()){

    fBPIDs->GetEntry(fEntry);
    while(!fFinalState->CheckForATopology()){
      fEntry++;
       fBPIDs->GetEntry(fEntry);
       if(fEntry==fReadTree->GetEntries()-1) break;//read last entry so can go to next chain     
    }
     fReadTree->GetEntry(fEntry);
    fEntry++;
  }
  else if(fChainFiles){
    if(fChainFileN<=fChainFiles->GetEntries()){//next file
      NextChainFile();  
      if(!fReadFile) return kFALSE;
      fBPIDs->GetEntry(fEntry);
      while(!fFinalState->CheckForATopology()){
	fEntry++;
	fBPIDs->GetEntry(fEntry);
        if(fEntry==fReadTree->GetEntries()-1) break;//read last entry so can go to next chain     
     }
      //fBParticles->GetEntry(fEntry);
      fReadTree->GetEntry(fEntry);
      fEntry++;
      
    }
  }
  else return kFALSE;
  return kTRUE; 
}
// Bool_t THSDataManager::ReadFinalStateEvent(Long64_t entry){
//   //Default reader for root files with vector<THSParticle*> branches
//   if(fEntry<fEntryList->GetN()){
//    fReadTree->LoadTree(fEntry);
//     Long64_t en=fEntryList->GetEntry(fEntry);
//     //  cout<<en<<" "<<fEntry<<" "<<fEntryList->Next()<<endl;
//     // Long64_t en=fReadTree->GetEntryList()->Next();
//     //fBPIDs->GetEntry(en);
//     //fBParticles->GetEntry(en);
//     fReadTree->GetEntry(en);
//     fEntry++;
//   }
//   else if(fChainFiles){
//     if(fChainFileN<=fChainFiles->GetEntries()){//next file
//       NextChainFile();  
//       if(!fReadFile) return kFALSE;
//       Long64_t en=fEntryList->GetEntry(fEntry);
//       fReadTree->GetEntry(en);
//        fEntry++;
      
//     }
//   }
//   else return kFALSE;
//   return kTRUE; 
// }
Bool_t THSDataManager::InitChain(TChain* chain){
  //loop over all files in chain
  fChainFiles=chain->GetListOfFiles();
  Info("THSDataManager::InitChain"," Will proceess all %d files in chain",fChainFiles->GetEntries());
  //Load first file
  fCurFileName=fChainFiles->At(fChainFileN++)->GetTitle();
  fTreeName=chain->GetName();
  Init(fCurFileName,chain->GetName());
  fCurFileName=gSystem->BaseName(fCurFileName);
  if(fOutDir!=TString("")){
    fCurFileName.Append(fFileAppend);
    InitOutput(fOutDir+"/"+fCurFileName);
  }
  return kTRUE;  
}


void THSDataManager::WriteParticles(){
  cout<<":WriteParticles() "<<fChainFiles<<" "<<fWriteTree<<endl;
  //Write all input events on 1 go
  if(!fChainFiles){
    if(fOutDir!=TString("")){
      fCurFileName.Append(fFileAppend);
      InitOutput(fOutDir+"/"+fCurFileName);
    }
  }
  while(ReadEvent()){
    if(fWriteTree->GetEntries()==100)
      fWriteTree->OptimizeBaskets();
    if(fWriteThis&&fWriteTree){fWriteTree->Fill();PostWrite();}
    fWriteThis=kTRUE;//assume will write next event unless told otherwise
  }
  if(fWriteTree)CloseOutput();
}
void THSDataManager::InitOutput(TString filename){
  fWriteThis=kTRUE;
  //Make output directory if not existing
  if(!fWriteFile) gSystem->Exec(Form("mkdir -p %s",fOutDir.Data()));
  fWriteFile=new TFile(filename,"recreate");
  //fWriteFile->SetCompressionSettings(401);//LZ4
  fWriteTree=new TTree("HSParticles","data tree");
  fWriteTree->Branch(fReadBName,&fParticles);
  fWriteTree->Branch("PIDs",&fPIDs);
  //Make an unsplit tree for MakeSelector
  TTree* UnSplitTree=new TTree("HSUnSplit","unsplit tree for MakeSelector");
  UnSplitTree->Branch(fReadBName,&fParticles,256000,0);
  UnSplitTree->Branch("PIDs",&fPIDs,64000,0);
  UnSplitTree->Write();
  delete UnSplitTree;
  
  if(fAddGenerated)fWriteTree->Branch(fWriteGName,&fGenerated);
  Info("THSDataManager::InitOutput","Saving particles to %s",filename.Data());
}
void THSDataManager::CloseOutput(){
  if(!fWriteFile) return;
  fWriteFile->cd();
  if(fWriteTree)fWriteTree->Write();
  fWriteFile->Close();
  delete fWriteFile;
  fWriteFile=nullptr;
  fWriteTree=nullptr;
}
void THSDataManager::PrintEvent(Long64_t entry){

  Info("PrintLundEvent","Printing First event, this should help configure project class");
  if(!ReadEvent(entry)) {Error("PrintEvent","error in event read");return;}  
  cout<<"Initialised with "<<fNin<<" particles"<<endl;
  for(UInt_t i=0;i<fNin;i++){
    fParticles[i].Print("");  
  }
  
}
void THSDataManager::PrintPerfStats(){
  if(fReadTree)fReadTree->PrintCacheStats();
  if(fPerfstats) fPerfstats->Print();
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
  //if(fWeights)fWeights->GetEntryBinarySearch((Long64_t)fTUID);
}
Double_t THSDataManager::GetWeight(Int_t species){
  if(fWeights) fWeight=fWeights->GetWeight(species);
  return fWeight;
}
