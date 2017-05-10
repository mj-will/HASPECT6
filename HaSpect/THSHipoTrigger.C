#include "THSHipoTrigger.h"

THSHipoTrigger::THSHipoTrigger(){
  fHipo->ConfigBank("FTCAL::hits");
  //Get the necessary items from FT Bank
  fFTCalBank=fHipo->GetBank("FTCAL::hits");

  fFTCal_E=fFTBank->GetItem("energy");
}
void THSHipoTrigger::InitOutput(TString filename){
  THSDataManager::InitOutput(filename);
  fWriteTree->Branch("FTEsum",&fFTEsum,"FTEsum/F");
  fWriteTree->Branch("PassTrig1",&fPassTrig1,"PassTrig1/B");
}

Bool_t THSHipoTrigger::ReadEvent(Long64_t entry){
  fPassTrig1=kFALSE;
  fFTEsum=0;
  
  if(!THSHipoReader::ReadEvent()) return kFALSE;

  //Now do some trigger simulations

  if(fFTCalBank){ //Get FTCAL hits information
    while(fFTBank->NextEntry()){ //iterate over hits
      fFTEsum+=fFTCal_E->Val(); //sum energy of each cluster
    }
  }

  if(fFTEsum>1) fPassTrig1=kTRUE;
  return kTRUE;
}
