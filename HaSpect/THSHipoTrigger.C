#include "THSHipoTrigger.h"

THSHipoTrigger::THSHipoTrigger(){
 
  
}
Bool_t THSHipoTrigger::Init(TString filename,TString name){

  THSHipoReader::Init(filename,name);
  
  if(!fRecEvBank){
    Info("THSHipoTrigger","Opened file");
    fHipo->ConfigOnlyBank("REC::Event");
    fHipo->ConfigOnlyBank("RUN::config");
    fHipo->ConfigBanks();
    
    fRecEvBank=fHipo->GetBank("REC::Event");
    fRunConBank=fHipo->GetBank("RUN::config");

    fRecEvNRun=fRecEvBank->GetItem("NRUN");
    fRecEvNEVENT=fRecEvBank->GetItem("NEVENT");
    fRecEvTYPE=fRecEvBank->GetItem("TYPE");
    fRecEvTRG=fRecEvBank->GetItem("TRG");
    fRecEvHelic=fRecEvBank->GetItem("Helic");
    fRecEvEVNTime=fRecEvBank->GetItem("EVNTime");
    fRecEvBCG=fRecEvBank->GetItem("BCG");
    fRecEvLT=fRecEvBank->GetItem("LT");
    fRecEvSTTime=fRecEvBank->GetItem("STTime");
    fRecEvRFTime=fRecEvBank->GetItem("RFTime");
    fRecEvPTIME=fRecEvBank->GetItem("PTIME");

    
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
  fWriteTree->Branch("EvTime",&fEvTime,"EvTime/F");
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
  /// initalize variables:
  fNRun=fRecEvNRun->Val();
  fNEvent=fRecEvNEVENT->Val();
  fType=fRecEvTYPE->Val();
  fTrig=fRecEvTRG->Val();
  fTrig=fRecEvHelic->Val();
  fEvTime=fRecEvEVNTime->Val();
  fBCG=fRecEvBCG->Val();
  fLT=fRecEvLT->Val();
  fSTTime=fRecEvSTTime->Val();
  fRFTime=fRecEvRFTime->Val();
  fPTime=fRecEvPTIME->Val();

  CreateBitPattern(fRunTrig->Val());
  fFTHigh=fTrigBits[30];
  fFTLow=fTrigBits[29];
  
  
  THSHipoReader::ReadEvent();
  return kTRUE;;

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
