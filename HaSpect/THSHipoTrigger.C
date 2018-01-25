#include "THSHipoTrigger.h"

THSHipoTrigger::THSHipoTrigger(){
   
  fHipo->ConfigBank("RUN::config");
  fRunConBank=fHipo->GetBank("Run::config");

  fRunTrig=fRunConBank->GetItem("trigger");
}
void THSHipoTrigger::InitOutput(TString filename){
  THSDataManager::InitOutput(filename);
  fWriteTree->Branch("FTHigh",&fFTHigh,"FTHigh/I");
  fWriteTree->Branch("FTLow",&fFTLow,"FTLow/I");
 }

Bool_t THSHipoTrigger::ReadEvent(Long64_t entry){
  /// initalize variables:
  fFTHigh=0;
  fFTLow=0;
 
  if(!THSHipoReader::ReadEvent()) return kFALSE;

  CreateBitPattern(fRunTrig->Val());
  fFTHigh=fTrigBits[30];
  fFTLow=fTrigBits[29];
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
