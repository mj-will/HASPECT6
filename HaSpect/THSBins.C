//#include "Riostream.h"
#include "THSBins.h"
#include "TROOT.h"
#include "TMath.h"
#include "TLeaf.h"
#include "TDirectory.h"
#include <algorithm>


ClassImp(THSBins)	

THSBins::THSBins(TString name) :TNamed(name,name),fNaxis(0),fNbins(0){
}

THSBins::THSBins(TString name,TString filename):TNamed(name,name),fBinTree(0),fFile(0),fNaxis(0),fNbins(0){
  TDirectory *saveDir=gDirectory;
  fFile=new TFile(filename);
  if(!fFile->IsOpen()) {Error("THSBins::THSBins(TString name,TString filename)"," File does not exist %s",filename.Data());return;}
  THSBins* filebins=(THSBins*)fFile->Get(name);
  if(!filebins) {Error("THSBins::THSBins(TString name,TString filename)","did not find bins %s in file %s",name.Data(),filename.Data());return;}
  fBinNames=filebins->GetBinNames();
  fNbins=filebins->GetN();
  fNaxis=filebins->GetNAxis();
  fVarAxis=filebins->GetVarAxis();
  //tree is not written to file as data member
  fBinTree=(TTree*)fFile->Get("HSBinTree");
  if(!fBinTree) cout<<"THSBins::THSBins(TString name,TString filename)  Tree does not exist "<<"HSBinTree"<<endl;
  saveDir->cd();
  // cout<<"DONE THSBins::THSBin"<<endl;
}
THSBins::THSBins(const THSBins& other, const char* name): TNamed(name,name),fBinTree(0),fFile(0),fNaxis(0),fNbins(0){
  fBinNames=other.fBinNames;
  fNbins=other.fNbins;
  fNaxis=other.fNaxis;
  fVarAxis=other.fVarAxis;
  if(other.fBinTree) fBinTree=(TTree*)other.fBinTree->Clone();
  fFile=0;

}
THSBins::~THSBins(){
  if(fBinTree) delete fBinTree;
  if(fFile){fFile->Close(); delete fFile;}
}
void THSBins::AddAxis(TString name,Int_t nbins,Double_t min,Double_t max){
  //Add a new axis for a given variable, name should be tree name
  //Want to make an array for use with FindBin(). must call the other contructor
  TArrayD bins(nbins+1);
  Double_t binwidth = (max - min) / Double_t(nbins);
  for(Int_t i=0;i<nbins+1;i++)
    bins[i]= min + i * binwidth;
  AddAxis(name,nbins,bins.GetArray());
  
  // TAxis axis(nbins,min,max);
  // axis.SetName(name);
  // fVarAxis.push_back(axis);
  // fNaxis++;
}
void THSBins::AddAxis(TString name,Int_t nbins,Double_t* xbins){
  //Add a new axis for a given variable, name should be tree name
  TAxis axis(nbins,xbins);
  axis.SetName(name);
  fVarAxis.push_back(axis);
  fNaxis++;
}
void THSBins::InitialiseBins(){
  if(fNaxis==0) return;
  //Make bin names for every individual bin
  TString binName;
  fNbins=0;
  fBinNames.clear();
  IterateAxis(0,binName);

}
void THSBins::InitialiseBinTree(TString name,TString filename){
  //SetName(name);
  Info("THSBins::InitialiseBinTree"," create bin tree and save in file %s",filename.Data());
  TDirectory *saveDir=gDirectory;
  if(filename!=TString(""))fFile=new TFile(filename,"recreate");
  fBinTree=new TTree("HSBinTree","Contains bin number for each event");
  fBinTree->Branch("Bin",&fBin,"Bin/I");
  InitialiseBins();
  saveDir->cd(); //lists are in file directory, go back to ROOT


}
void THSBins::IterateAxis(Int_t iA,TString binName) {
  //iterate through all bins possible with given axis and constuct
  //unique names for them
  if (iA >= fNaxis){ //stop clause
    fBinNames.push_back(binName);
    fNbins++;
    //Info("THSBins::IterateAxis"," %s",binName.Data());
    return;
  }
  VecString_t part;
  for (int iB = 1; iB <= fVarAxis[iA].GetNbins(); iB++) { 
    fVarAxis[iA].SetBinLabel(iB,Form("%1.2f_",fVarAxis[iA].GetBinCenter(iB)));
    part.push_back(TString(fVarAxis[iA].GetName())+fVarAxis[iA].GetBinLabel(iB));
    IterateAxis(iA+1,binName+fVarAxis[iA].GetName()+fVarAxis[iA].GetBinLabel(iB));
   }
  if(std::find(fPartName.begin(),fPartName.end(),part)==fPartName.end()) {
    fPartName.insert(fPartName.begin(),part);//for correct ordering with fVar.Axis vector
  }
}

void THSBins::RunBinTree(TTree* tree){
  //create entry lists for tree
  TDirectory *saveDir=gDirectory;
  fFile->cd();
  TVectorD vVal(fNaxis);//values of variables for given entry
  vector<Int_t> vValI(fNaxis);//int values of variables for given entry
  vector<Int_t> vIntIndex;
  
  Bool_t GotAnInt=kFALSE;
  
  tree->SetBranchStatus("*","0");//faster if turnoff unused branches
  for(Int_t j=0;j<fNaxis;j++){
    tree->SetBranchStatus(fVarAxis[j].GetName(),"1");//STATUS must be called before ADDRESS!! see Important remarkse in TChain SetBranchStatus!
    tree->GetLeaf(fVarAxis[j].GetName())->GetTypeName();//this isneeded to check type properly when setting branch address
    
    if(tree->SetBranchAddress(fVarAxis[j].GetName(),&vVal[j])==-2){
      //In case we have Int_t branches
      if(tree->SetBranchAddress(fVarAxis[j].GetName(),&vValI[j])==0){
	vIntIndex.push_back(j);
	GotAnInt=kTRUE;
      }
    }
  }
  vector<Int_t> vBin(fNaxis); //store for the bin number of each axis
  for(Long64_t i=0;i<tree->GetEntries();i++){//loop over events
    tree->GetEntry(i);
    if(GotAnInt){//put the integer value in the double array
      for(UInt_t iv=0;iv<vIntIndex.size();iv++){
	vVal[vIntIndex[iv]]=vValI[vIntIndex[iv]];
     }
    }
    fBin=FindBin(vVal);
    fBinTree->Fill();
  }
  tree->SetBranchStatus("*",1);
  tree->ResetBranchAddresses();
  saveDir->cd();

}

TTree* THSBins::GetBinnedTree(TTree* tree,Int_t bin){
  //This class will not delete the tree
  // gROOT->cd();
  TDirectory *saveDir=gDirectory;
  tree->AddFriend(fBinTree,"bt");
  tree->SetBranchStatus("bt.Bin",1);
  if(tree->GetDirectory()) tree->GetDirectory()->cd();//or else complains memeiry resident...
  TTree* tree_copy=tree->CopyTree(Form("bt.Bin==%d",bin));
  tree_copy->SetName(fBinNames[bin]);
  //tree_copy->SetTitle(fBinNames[bin]);
  tree->RemoveFriend(fBinTree);
  tree_copy->RemoveFriend(fBinTree);
  tree->GetListOfFriends()->RemoveAll();
  tree_copy->GetListOfFriends()->RemoveAll();
  saveDir->cd();
  return tree_copy;
 }
void THSBins::Save(){
  Info(" THSBins::Save()"," Saving %s",GetName());
  if(!fFile) return;
  fFile->cd();
  if(fBinTree){ fBinTree->Write();delete fBinTree;fBinTree=0;}
  Write();
  fFile->Close();
  if(fFile) delete fFile;
  
}
void THSBins::PrintAxis(){

  for(Int_t iA=0;iA<fNaxis;iA++)
    cout<<fVarAxis[iA].GetName()<<" "<<fVarAxis[iA].GetNbins()<<" "<<fVarAxis[iA].GetXmin()<<" "<<fVarAxis[iA].GetXmax()<<" "<<endl;
}

// TEntryList* THSBins::MergedLists(TList* list){

//   TList* mlist=new TList(); 
//   TEntryList* mel=0;
//   for(Int_t i=0;i<list->GetEntries();i++){
//     if(mel=dynamic_cast<TEntryList*>(fEntryLists->FindObject(list->At(i)->GetName())))
//       mlist->Add(mel);

//   }
//   TEntryList *subList=new TEntryList();
//   subList->Merge(mlist);
//   delete mlist;
//   return subList; 

// }

Int_t THSBins::FindBin(Double_t v0){
  TVectorD vals(1);vals[0]=v0;
  return FindBin(vals);
}
Int_t THSBins::FindBin(Double_t v0,Double_t v1){
  TVectorD vals(2);vals[0]=v0;vals[1]=v1;
  return FindBin(vals);
}
Int_t THSBins::FindBin(Double_t v0,Double_t v1,Double_t v2){
  TVectorD vals(3);vals[0]=v0;vals[1]=v1;vals[2]=v2;
  return FindBin(vals);
}
Int_t THSBins::FindBin(Double_t v0,Double_t v1,Double_t v2,Double_t v3,Double_t v4,Double_t v5){
  TVectorD vals(6);vals[0]=v0;vals[1]=v1;vals[2]=v2;vals[3]=v3;vals[4]=v4;vals[5]=v5;
  return FindBin(vals);
}
Int_t THSBins::FindBin(TVectorD vals){
  //Loop over each axis and find bin for each
  Bool_t InLimits=kTRUE;
  for(Int_t iA=0;iA<fNaxis;iA++)//first check var is within variable ranges
    if(vals[iA]<fVarAxis[iA].GetXmin()||vals[iA]>fVarAxis[iA].GetXmax()) InLimits=kFALSE;
  if(!InLimits) {return -1;}
  //now find bin for each axis
  vector<Int_t> vBin(fNaxis); //store for the bin number of each axis
  for(Int_t iA=0;iA<fNaxis;iA++){//loop over vars/axis
      vBin[iA]=1+TMath::BinarySearch(fVarAxis[iA].GetNbins(),fVarAxis[iA].GetXbins()->GetArray(),vals[iA]);
  }
  //now have the bin for each axis, find single bin
  Int_t theBin=-1;
  for(Int_t iA1=0;iA1<fNaxis-1;iA1++){
    Int_t tbin=vBin[iA1]-1;//-1 as bin indexing starts at 1 with 0 underflow
    for(Int_t iA2=iA1+1;iA2<fNaxis;iA2++)
      tbin*=fVarAxis[iA2].GetNbins();
    theBin+=tbin;
  }
  theBin+=vBin[fNaxis-1];
  return theBin;
}

