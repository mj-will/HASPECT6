#include "THSHipoTrigger.h"

THSHipoTrigger::THSHipoTrigger(){
 
  
}
Bool_t THSHipoTrigger::Init(TString filename,TString name){

  if(!fRunConBank){
    //Add the trigger banks to those to be configured
    Info("THSHipoTrigger","Opened file");
    fHipo->ConfigOnlyBank("RUN::config");
    fHipo->ConfigOnlyBank("RAW::scaler");
  }
  THSHipoReader::Init(filename,name);
  
  if(!fRunConBank){
    
    fRunConBank=fHipo->GetBank("RUN::config");
 
    fRecEvNRun=dynamic_cast<THipoItemI*>(fEvBank->GetItem("NRUN"));
    fRecEvNEVENT=dynamic_cast<THipoItemI*>(fEvBank->GetItem("NEVENT"));
    fRecEvTYPE=dynamic_cast<THipoItemB*>(fEvBank->GetItem("TYPE"));
    fRecEvTRG=dynamic_cast<THipoItemL*>(fEvBank->GetItem("TRG"));
    fRecEvHelic=dynamic_cast<THipoItemB*>(fEvBank->GetItem("Helic"));
    fRecEvEVNTime=dynamic_cast<THipoItemF*>(fEvBank->GetItem("EVNTime"));
    fRecEvBCG=dynamic_cast<THipoItemF*>(fEvBank->GetItem("BCG"));
    //    fRecEvLT=fEvBank->GetItem("LT");
    //fRecEvSTTime=fEvBank->GetItem("STTime");
    fRecEvRFTime=dynamic_cast<THipoItemF*>(fEvBank->GetItem("RFTime"));
    fRecEvPTIME=dynamic_cast<THipoItemF*>(fEvBank->GetItem("PTIME"));

    
    fRunTrig=dynamic_cast<THipoItemL*>(fRunConBank->GetItem("trigger"));

    
    fRawScalBank=fHipo->GetBank("RAW::scaler");
    
    fRawScalChan=dynamic_cast<THipoItemS*>(fRawScalBank->GetItem("channel")); 
    fRawScalSlot=dynamic_cast<THipoItemB*>(fRawScalBank->GetItem("slot"));
    fRawScalVal=dynamic_cast<THipoItemI*>(fRawScalBank->GetItem("value"));
    fRawScalHel=dynamic_cast<THipoItemB*>(fRawScalBank->GetItem("helicity"));
  }

  fCharge=0;
  fHelicity=-1;
  fTotCharge=0;
  fNScalerReads=0;
  return kTRUE;
}

void THSHipoTrigger::InitOutput(TString filename){
  THSDataManager::InitOutput(filename);
  fWriteTree->Branch("TrigFT",&fTrigFT,"TrigFT/I");
 

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
 
  fWriteTree->Branch("TotCharge",&fTotCharge,"TotCharge/F");
  fWriteTree->SetBranchStatus("TotCharge",0); //only write total charge at end
  fWriteTree->Branch("Helicity",&fHelicity,"Helicity/I");
 }

Bool_t THSHipoTrigger::ReadEvent(Long64_t entry){

  //cout<<"THSHipoTrigger::ReadEvent("<<endl;    

  //Note include an extra fill in case there is an extra scaler current
  if(!fHipo->NextEvent()) {
    //End of file write the total current
    if(fWriteTree){
      fWriteTree->SetBranchStatus("*",0);
      fWriteTree->SetBranchStatus("TotCharge",1);
      fWriteTree->Fill();
      fWriteTree->SetBranchStatus("*",1);
    }
    cout<<"THSHipoTrigger::ReadEvent total charge for this file "<<fTotCharge<<endl;
    cout<<"  at average of current of "<<fTotCharge/fNScalerReads/0.033<<"nA per read. "<<endl; 
    return kFALSE;
  }

  
  fEntry++;
 
  fWriteThis=kFALSE; //don't write scaler events on their own, accumulate and write at end or with other events
  RawScaler();
  
  //FT trigger
  //cout<<"entry "<<fRunConBank->GetEntry()<<endl;
  fRunConBank->NextEntry(); //Get RunCon bank as it is not in HipoReader
  if(fRunConBank->GetEntry()<0) return kTRUE; 

  CreateBitPattern(fRunTrig->Val());
  fTrigFT=0;
  if(fTrigPeriod==0){//Engineering
    fTrigFT+=fTrigBits[30]*1;
    fTrigFT+=fTrigBits[29]*2;
  }
  else if(fTrigPeriod==1){//RG-A (02/18)
    fTrigFT+=fTrigBits[23]*8;
    fTrigFT+=fTrigBits[25]*4;
    fTrigFT+=fTrigBits[26]*2;
    fTrigFT+=fTrigBits[30]*1;
  }
  //Apply filter on FT trigger
  //fSofFTTrig==1 
  if(fSoftFTTrig==1&&!fTrigFT){fWriteThis=kFALSE;return kTRUE;}
  fWriteThis=kTRUE;
  //Similarily using fWriteThis can apply other trigger filters

  //Now check Event Builder Banks, -2 =>we have all ready got event
  //Note that this funtion will call fEvBank->NextEntry()
  THSHipoReader::ReadEvent(-2); 

  //now other event scalars
  if(fEvBank->GetEntry()<0) return kTRUE;

  fNRun=fRecEvNRun->Val();
  fNEvent=fRecEvNEVENT->Val();
  fType=fRecEvTYPE->Val();
  fTrig=fRecEvTRG->Val();
  fTrig=fRecEvHelic->Val();
  fEventTime=fRecEvEVNTime->Val();
  fBCG=fRecEvBCG->Val();
  //fLT=fRecEvLT->Val();
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
void  THSHipoTrigger::RawScaler()
{
  Double_t GatedFC=0;
  Double_t UnGatedFC=0;

  while(fRawScalBank->NextEntry()){
    if(fRawScalBank->GetEntry()<0) break;
    
    if(fRawScalChan->Val()==0 && fRawScalSlot->Val()==0)
      GatedFC=fRawScalVal->Val();
    if(fRawScalChan->Val()==0 && fRawScalSlot->Val()==1)
      UnGatedFC=fRawScalVal->Val();

    if(fRawScalBank->GetEntry()==0) fHelicity=fRawScalHel->Val(); //this should be the same value for all entries...
  }
  if(GatedFC==0) return;
  
  if(fUseUnGated) GatedFC=UnGatedFC-GatedFC;
  Float_t trueFreq = GatedFC / (0.03333 - 0.0005);
  Float_t beamCurrent = (trueFreq-100)/906.2/fCurFactor;

  fCharge=beamCurrent*0.033;//0.033ms scaler read
  fTotCharge+=fCharge;
  fNScalerReads++;

}
void  THSHipoTrigger::PostWrite(){
  //fCharge=0; //reset charge 
  //Leave helicity as it is
}
