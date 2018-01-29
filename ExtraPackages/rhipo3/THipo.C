#include "THipo.h"
#include <TSystem.h>
#include <TApplication.h>
#include <TObjString.h>
#include <TObjArray.h>


THipo::THipo(){
  fParser=new THipoBankParser();
  
  //Decide whether input files or dirs from command line
  Int_t argc=gApplication->Argc();
  char** argv=gApplication->Argv();
  for(Int_t i=1;i<argc;i++){
    TString dirname=argv[i];
    if(dirname.Contains("--in=")){
      fUseCLDirs=kTRUE;
      break;
    }
    else
      fUseCLDirs=kFALSE;
  }
  //Look for output dir
  for(Int_t i=1;i<argc;i++){
    TString dirname=argv[i];
    if(dirname.Contains("--out=")){
      dirname.Remove(0,6);
      fOutDirName=dirname;
      cout<<"THipo will write root files to "<<fOutDirName<<endl;
      if(gSystem->MakeDirectory(fOutDirName)==-1){
	cout<<"Error directory "<<fOutDirName<<" already exists or its parent directory does not exist"<<endl;
	exit(1);
      }
    }
  }
  
}
THipo::THipo(TString filename){
  fParser=new THipoBankParser();
}
THipo::~THipo(){
  if(fParser) delete fParser;
  for(UInt_t ib=0;ib<fBanks.size();ib++)
    delete fBanks.at(ib);
  fBanks.clear();
  
}
void THipo::InitFile(TString filename){
  fReader.open(filename.Data());
  fReader.showInfo();
  //  fReader.readRecordIndex();
  fNRecords = fReader.getRecordCount();
  fRecordCount=0;
}
void THipo::AddCommandLineFiles(){
  //If using command line dirs do that then return
  if(fUseCLDirs) //This is set in the constructor
    return AddCommandLineDirs();
  //look for files from command line
  cout<<"THipo::AddCommandLineFiles() "<<" using input files from commandline"<<endl;
  Int_t argc=gApplication->Argc();
  char** argv=gApplication->Argv();
  for(Int_t i=0;i<argc;i++){
    if(TString(argv[i]).Contains(".hipo")) fInFiles.push_back(argv[i]);
  } 
}
void THipo::AddCommandLineDirs(){
  cout<<"THipo::AddCommandLineDirs() "<<" using input directory from commandline"<<endl;
  //look for files from command line
  Int_t argc=gApplication->Argc();
  char** argv=gApplication->Argv();
  for(Int_t i=1;i<argc;i++){
    TString dirname=argv[i];
    if(dirname.Contains("--in=")){
      dirname.Remove(0,5);
      cout<<"THipo::AddCommandLineDirs() checking for files in "<<dirname<<endl;
      void *dir=gSystem->OpenDirectory(dirname);
      cout<<dir<<endl;
      TString fileName;
      while( (fileName=(gSystem->GetDirEntry(dir)))){
	cout<<fileName<<endl;
 	if(fileName.Contains(TString(".hipo")))
	  fInFiles.push_back(dirname+fileName);
 	if(fileName==TString("")) break;
     }
    }
    
  }
  
  cout<<"THipo::AddCommandLineDirs() found "<<fInFiles.size()<<" hipo files in --in directories"<<endl;
  
 
}

void THipo::ToRoot(){
  //Use file list
  for(UInt_t i=0;i<fInFiles.size();i++)
    ToRoot(fInFiles.at(i));

}
void THipo::ToRoot(TString filename){
  cout<<"THipo::ToRoot "<<filename<<endl;
  //Convert file called filename
  InitFile(filename);
  cout<<"THipo::ToRoot Number of records "<<fNRecords<<endl;
  //if perviously defined by user configuration or previous hipo file
  if(fBanks.size()==0)ConfigBanks();
  // if(fBanks.size()==0)ConfigAllBanks();
  //filename.ReplaceAll(".hipo.3",".root");
  // TString append=filename(filename.Last('.')+1,filename.Sizeof());
  // if(append!=TString("hipo")
  //    filename=filename(0,filename.Last('.'));
  filename.ReplaceAll(".hipo",".root");

  //Append the output directory name
  if(fOutDirName.Sizeof()>1){
    TString newfilename=fOutDirName+"/"+gSystem->BaseName(filename);
    filename=newfilename;
  }
  InitOutput(filename);
  //loop over events
  while(NextEvent());
  CloseOutput();
  
}
Bool_t  THipo::NextFile(){
  if(fFilei>=fInFiles.size()) return kFALSE;
  cout<<" THipo::NextFile() Analyse file "<<fInFiles.at(fFilei)<<endl;
  InitFile(fInFiles.at(fFilei++));
  return kTRUE;
}
Long64_t THipo::ProcessFile(TString filename){
  InitFile(filename);
  //cout<<"Number of records "<<fNRecords<<endl;

  //loop over records
  while(NextEvent());
  if(fOutTree) CloseOutput();
  return fNEvents;
}
Bool_t THipo::NextRecord(){//Move to next record in file
  if(fRecordCount>=fNRecords) return kFALSE;
  fReader.readRecord(fRecord,fRecordCount++);
  fNRecEvents = fRecord.getEventCount(); //get events in this record
  fRecEventCount=0;//restart record event counter
  // cout<<"Number of rec events "<<fNRecEvents<<" in record "<<fRecordCount<<endl;
  return kTRUE;
}
Bool_t THipo::NextEvent(){//Read next event in file
 if(fRecEventCount>=fNRecEvents){//Look for next record
    if(!NextRecord()) return kFALSE;//No more records
  }
 // fHipoEvent = fRecord.getHipoEvent(fRecEventCount++);
 fRecord.readHipoEvent(fHipoEvent,fRecEventCount++);
  //Loop over group
  for(UInt_t ig=0;ig<fBanks.size();ig++){
    fCurBank=fBanks.at(ig);
    fCurBank->ClearEvent();
    //Loop over int items
    for(UInt_t in=0;in<fCurBank->NInt();in++){
      ReadItemI(in);
    }
    //Loop over float items
    for(UInt_t fn=0;fn<fCurBank->NFloat();fn++)
      ReadItemF(fn);
    
  }//end group loop
  if(fOutTree)fOutTree->Fill();
  fNEvents++;
  return kTRUE;
}
Bool_t THipo::ConfigBank(TString bankname){
  fBanks.push_back(new THipoBank(bankname));
  Bool_t isOK= fParser->ConfigBank3(fBanks.back());
  fBankMap[fBanks[fBanks.size()-1]->GetName()]=fBanks.size()-1;//map name after configure in case items removed
  return isOK;
}
Bool_t THipo::ConfigBanks(){
  //Use Hipo Dictionary to configure banks
  hipo::record dictionary;
  fReader.readHeaderRecord(dictionary);
  printf(" DICTIONARY READ COUNT = %d\n",dictionary.getEventCount());
  //  Loop through dictionary and get the bank information
  hipo::event  schema;
  for(int d = 0; d < dictionary.getEventCount(); d++){
    dictionary.readHipoEvent(schema,d);
    //printf("Dictionary event %d is read\n",d);
    std::string schemaString = schema.getString(31111,1);
    //printf("schema : %s\n",schemaString.c_str());
    if(fOnlyBanks.size()!=0){
      TString dBank=schemaString;
      for(UInt_t i=0;i<fOnlyBanks.size();i++)
	if(dBank.Contains(fOnlyBanks[i]))
	  ConfigBank(schemaString.data());
      
    }
    else
      ConfigBank(schemaString.data());
  }
  
  return kTRUE;
}
Bool_t THipo::ConfigOnlyBank(TString bankname){
  fOnlyBanks.push_back(bankname);
  return kTRUE;
}
void THipo::InitOutput(TString filename){
  fOutFile=new TFile(filename,"recreate");
  fOutFile->SetCompressionLevel(fCompression);
  fOutTree=new TTree("hipo2root","CLAS12 banks in ROOT");
  //Make branches from all the banks
  for(UInt_t ib=0;ib<fBanks.size();ib++)
    fBanks.at(ib)->InitTree(fOutTree);
  // fOutTree->Print("");
}
void THipo::CloseOutput(){
  fOutTree->Write();
  fOutFile->Close();
  delete fOutFile;fOutFile=nullptr;
  fOutTree=nullptr;
}
THipoBank* THipo::GetBank(TString name){
  auto it=fBankMap.find(name);
  if(it==fBankMap.end()){
    cout<<"Error THipo* GetBank( No bank configured with name = "<<name<<endl;
    return nullptr;
  }
  return fBanks[fBankMap[name]];
};

////////////////////////////////////////////////THipoBank
THipoBank::~THipoBank(){
   for(UInt_t i=0;i<fVecItems.size();i++) delete fVecItems.at(i);
   for(UInt_t i=0;i<fVecI.size();i++) delete fVecI.at(i);
   for(UInt_t i=0;i<fVecF.size();i++) delete fVecF.at(i);
   fVecItems.clear();
   fVecI.clear();
   fVecF.clear();
   fItemsI.clear();
   fTypeI.clear();
   fItemsF.clear();
}
void THipoBank::AddIntItem(TString name,Int_t id,TString type){
  fItemsI.push_back(name);
  fTypeI.push_back(type);
  fVecI.push_back(new vector<Long_t >);
  //  fVecI.push_back(new vector<Int_t >);
  fItemMap[name]=fIndexI.size();
  fIndexI.push_back(id);
  fFloatMap[name]=kFALSE;
}
void THipoBank::AddFloatItem(TString name,Int_t id){
  fItemsF.push_back(name);
  fVecF.push_back(new vector<Float_t >);
  fItemMap[name]=fIndexF.size();
  fIndexF.push_back(id);
  fFloatMap[name]=kTRUE;
}
void THipoBank::InitTree(TTree* tree){
  //Make a branch for each integer item
  for(UInt_t in=0;in<NInt();in++){
    TString branchname=fName;
    branchname.ReplaceAll("::","_");
    branchname.ReplaceAll(" ","");
    // if(fTypeI[in].Contains("SHORT")){
    //   vector<Short_t>* vecptr=reinterpret_cast<vector<Short_t >*>(fVecI.at(in));
    //   tree->Branch(branchname+"_"+fItemsI.at(in),vecptr);
    // }
    // else if(fTypeI[in].Contains("BYTE")){
    //   vector<Char_t>* vecptr=reinterpret_cast<vector<Char_t >*>(fVecI.at(in));
    //   tree->Branch(branchname+"_"+fItemsI.at(in),vecptr);
    // }
    // else if(fTypeI[in].Contains("INT")){
    //   vector<Int_t>* vecptr=reinterpret_cast<vector<Int_t >*>(fVecI.at(in));
    //   tree->Branch(branchname+"_"+fItemsI.at(in),vecptr);
    // }
    // else {
    //   vector<Long_t>* vecptr=(fVecI.at(in));
    //   tree->Branch(branchname+"_"+fItemsI.at(in),vecptr);
    // }
    //vector<Long_t>* vecptr=(fVecI.at(in));
    // tree->Branch(branchname+"_"+fItemsI.at(in),vecptr);
    tree->Branch(branchname+"_"+fItemsI.at(in),&(fVecI.at(in)));   
  }
  //Make a branch for each float item
  for(UInt_t fn=0;fn<NFloat();fn++){
    TString branchname=fName;
    branchname.ReplaceAll("::","_");
    branchname.ReplaceAll(" ","");
    tree->Branch(branchname+"_"+fItemsF.at(fn),(fVecF.at(fn)));
  }
  
}
void THipoBank::Print(){
  cout<<"Printing Hipo Bank"<<endl;
  cout<<"Name = "<<fName<<endl;
  cout<<"Group = "<<fGroup<<endl;
  cout<<"Int Items :"<<endl;
  for(UInt_t i=0;i<fItemsI.size();i++)
    cout<<"  "<<fItemsI[i]<<" with type "<<fTypeI[i]<<endl;
  cout<<"Float Items :"<<endl;
  for(UInt_t i=0;i<fItemsF.size();i++)
    cout<<"  "<<fItemsF[i]<<endl;
}
THipoItem* THipoBank::GetItem(TString name){
  if((std::find(fItemsI.begin(),fItemsI.end(),name)==fItemsI.end())&&(std::find(fItemsF.begin(),fItemsF.end(),name)==fItemsF.end())){
    cout<<"Error in THipoItem THipoBank::GetItem item "<<name<<" is not configured  in bank "<<GetName()<<endl;
    exit(0);
  }
  Int_t index=fItemMap[name];
  if(fFloatMap[name])
    fVecItems.push_back(new THipoItem(fVecF[index],&fEntry));
  else
    fVecItems.push_back(new THipoItem(fVecI[index],&fEntry));
  return fVecItems.back();
}
////////////////////////////////////////////////THipoBankParser
THipoBankParser::THipoBankParser(){


}

Bool_t THipoBankParser::ConfigBank3(THipoBank* bank){

  //Decode the dictionary string into bank; group; and item
  TObjArray* selectname=bank->GetName().Tokenize("}");
  TString bankname(selectname->At(0)->GetName());
  TString groupname=bankname(1,bankname.First(",")-1);
  bankname=bankname(bankname.First(",")+1,bankname.Sizeof());//just get name
  bank->SetName(bankname);
  TObjArray* use_var=nullptr;
  if(selectname->GetEntries()>1)
    use_var=((TObjString*)selectname->At(1))->String().Tokenize("][");

  cout<<"THipoBankParser::ConfigBank "<<bank->GetName()<<endl;
  bank->SetGroup(groupname.Atoi());
  //Find the items
  for(Int_t ivar=0;ivar<use_var->GetEntries();ivar++){

    TObjArray* item=dynamic_cast<TObjString*>(use_var->At(ivar))->String().Tokenize(",");
    
    TString item_type=((TObjString*)item->At(2))->String();
    TString item_name=((TObjString*)item->At(1))->String();
    TString item_id=((TObjString*)item->At(0))->String();
     if(item_type.Contains("FLOAT")||item_type.Contains("DOUBLE")||item_type.Contains("VECTOR3F"))
      bank->AddFloatItem(item_name,item_id.Atoi());
    else 
      bank->AddIntItem(item_name,item_id.Atoi(),item_type);

    delete item; item=nullptr;
    
  }
  
  bank->Print();

  delete use_var;use_var=nullptr;
  delete selectname;selectname=nullptr;
  
  return kTRUE;
}
