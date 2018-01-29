#include "THSHipoTrigger.h"

THSHipoTrigger::THSHipoTrigger(){
 
  
}
Bool_t THSHipoTrigger::Init(TString filename,TString name){

  if(!fRunConBank){
    //Add the trigger banks to those to be configured
    Info("THSHipoTrigger","Opened file");
    fHipo->ConfigOnlyBank("RUN::config");
  }
  THSHipoReader::Init(filename,name);
  
  if(!fRunConBank){
    
    fRunConBank=fHipo->GetBank("RUN::config");
 
    fRecEvNRun=fEvBank->GetItem("NRUN");
    fRecEvNEVENT=fEvBank->GetItem("NEVENT");
    fRecEvTYPE=fEvBank->GetItem("TYPE");
    fRecEvTRG=fEvBank->GetItem("TRG");
    fRecEvHelic=fEvBank->GetItem("Helic");
    fRecEvEVNTime=fEvBank->GetItem("EVNTime");
    fRecEvBCG=fEvBank->GetItem("BCG");
    fRecEvLT=fEvBank->GetItem("LT");
    fRecEvSTTime=fEvBank->GetItem("STTime");
    fRecEvRFTime=fEvBank->GetItem("RFTime");
    fRecEvPTIME=fEvBank->GetItem("PTIME");

    
    fRunTrig=fRunConBank->GetItem("trigger");
  }
  return kTRUE;
}

void THSHipoTrigger::InitOutput(TString filename){
  THSDataManager::InitOutput(filename);
  fWriteTree->Branch("FTHigh",&fFTHigh,"FTHigh/I");
  fWriteTree->Branch("FTLow",&fFTLow,"FTLow/I");


  fWriteTree->Branch("NRun",&fNRun,"NRun/I");
  fWriteTree->Branch("NEvent",&fNEvent,"NEvent/I");
  fWriteTree->Branch("EvTime",&fEventTime,"EvTime/F");
  fWriteTree->Branch("Type",&fType,"Type/I");
  fWriteTree->Branch("Trig",&fTrig,"Trig/I");
  fWriteTree->Branch("BCG",&fBCG,"BCG/F");
  fWriteTree->Branch("LT",&fLT,"LT/F");
  fWriteTree->Branch("STTime",&fSTTime,"STTime/F");
  fWriteTree->Branch("RFTime",&fRFTime,"RFTime/F");
  fWriteTree->Branch("Helic",&fHelic,"Helic/I");
  fWriteTree->Branch("PTime",&fPTime,"PTime/F");
 
 }

Bool_t THSHipoTrigger::ReadEvent(Long64_t entry){

  //cout<<"THSHipoTrigger::ReadEvent("<<endl;    

  if(!fHipo->NextEvent()) return kFALSE;
  fEntry++;
  fRunConBank->NextEntry(); //Get RunCon bank as it is not in HipoReader
  //  cout<<"Bit Pattern "<<endl;
  // fRunConBank->Print();
  //FT trigger
  //cout<<"entry "<<fRunConBank->GetEntry()<<endl;
  if(fRunConBank->GetEntry()<0) return kTRUE; 

  CreateBitPattern(fRunTrig->Val());
  fFTHigh=fTrigBits[30];
  fFTLow=fTrigBits[29];
  //cout<<"Try trigger"<<endl;
  // if(fEntry>3) exit(0);
  //return kTRUE;
  //Apply filter on FT trigger
  //fSofFTTrig==1 either Low or High
  //fSofFTTrig==2 only High
  if(fSoftFTTrig==1&&!fFTLow&&!fFTHigh){fWriteThis=kFALSE;return kTRUE;}
  if(fSoftFTTrig==2&&!fFTHigh){fWriteThis=kFALSE;return kTRUE;}

  //Similarily using fWriteThis can apply other trigger filters
  //cout<<"HipoReder"<<endl;
  //Now check Event Builder Banks, -1 =>we have all ready got event
  //Note that this will call fEvBank->NextEntry()
  THSHipoReader::ReadEvent(-1); 

  //cout<<"READING SCALARS"<<endl;
  //now other event scalars
  //  fEvBank->NextEntry();
  //  fEvBank->Print();
  //fEvBank->Print();
  if(fEvBank->GetEntry()<0) return kTRUE;

  cout<<"WWWWWWWWWWWWWWWWWWWWWWWWWWW "<<fEvBank->GetEntry()<<" "<<fRecEvNRun->GetBankEntry()<<endl;
  fNRun=fRecEvNRun->Val();
  fNEvent=fRecEvNEVENT->Val();
  fType=fRecEvTYPE->Val();
  fTrig=fRecEvTRG->Val();
  fTrig=fRecEvHelic->Val();
  fEventTime=fRecEvEVNTime->Val();
  fBCG=fRecEvBCG->Val();
  fLT=fRecEvLT->Val();
  fSTTime=fRecEvSTTime->Val();
  fRFTime=fRecEvRFTime->Val();
  fPTime=fRecEvPTIME->Val();

  
  
  return kTRUE;

}
/////////////////////////////////////////////////////
///function to convert trigger bit pattern
void  THSHipoTrigger::CreateBitPattern(long val)
{
   unsigned int mask = 1 << (sizeof(int) * 8 - 1);

   Int_t Nbits=sizeof(int) * 8;
   
   for(int i = 0; i <Nbits; i++){
     
     if( (val & mask) == 0 ){
       fTrigBits[Nbits-i-1]=0;
     }
     else{
       fTrigBits[Nbits-i-1]=1;    
     }
     mask  >>= 1;
   }
}
