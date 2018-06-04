/**
	\class THSHipoTrigger
	
	Class to fill once per run THSRunInfo and 
	once per event THSEventInfo from hipo files.
	
	Inherits from THSHipoReader which sorts the THSParticle vector
	for the event tracks.

	Mainly uses REC::Event and RAW::Scaler banks.

	This class uses the stand-alone THipo library to interface
	to the values in the banks

*/
#include "THSHipoTrigger.h"

THSHipoTrigger::THSHipoTrigger(){
  fRunInfo=new THSRunInfo();
  fEventInfo=new THSEventInfo();
}

/////////////////////////////////////////////////////////////
/// Configure the REC::Event and RAW::Scaler banks
/// Additional items may be added in a similar fashion
Bool_t THSHipoTrigger::Init(TString filename,TString name){

  if(!fRawScalBank){
    //Add the trigger banks to those to be configured
    Info("THSHipoTrigger","Opened file");
    fHipo->ConfigOnlyBank("RAW::scaler");
  }
  THSHipoReader::Init(filename,name);
  
  if(!fRecEvNRun){
    
  
    fRecEvNRun=dynamic_cast<THipoItemI*>(fEvBank->GetItem("NRUN"));
    fRecEvNEVENT=dynamic_cast<THipoItemI*>(fEvBank->GetItem("NEVENT"));
    fRecEvTYPE=dynamic_cast<THipoItemB*>(fEvBank->GetItem("TYPE"));
    fRecEvTRG=dynamic_cast<THipoItemL*>(fEvBank->GetItem("TRG"));
    fRecEvHelic=dynamic_cast<THipoItemB*>(fEvBank->GetItem("Helic"));
    fRecEvRFTime=dynamic_cast<THipoItemF*>(fEvBank->GetItem("RFTime"));
  
    
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
  //  fWriteTree->Branch("TrigFT",&fTrigFT,"TrigFT/L");
 }

/////////////////////////////////////////////////////////
/// Read the event from THipo.
/// Write RunInfo at the end of file (when the gated charge is known).
/// Call THSHipoReader::ReadEvent to fill event THSParticle vector.
/// Fill the EventInfo from REC::Event.
Bool_t THSHipoTrigger::ReadEvent(Long64_t entry){

  cout<<"THSHipoTrigger::ReadEvent("<<endl;    

  //Note include an extra fill in case there is an extra scaler current
  if(!fHipo->NextEvent()) {
    //End of file write the total current
    if(fRunTree){
      fEvBank->SetEntry(0);
      fRunInfo->SetNRun(fRecEvNRun->Val());
      fRunInfo->SetType(fRecEvTYPE->Val());
      fRunInfo->SetTotalCharge(fTotCharge);
      fRunInfo->SetMeanCurrent(fTotCharge/fNScalerReads/0.033);
      fRunTree->Fill();
    }
    cout<<"THSHipoTrigger::ReadEvent total charge for this file "<<fTotCharge<<endl;
    //cout<<"  at average of current of "<<fTotCharge/fNScalerReads/0.033<<"nA per read. "<<endl; 

    if(fChainFiles){
      if(!NextChainFile())//check if another file and initilaise
	return kFALSE; //end of all files
      else fHipo->NextEvent(); //first event of new file
    }
    else return kFALSE; //Only analysing 1 file
  }


  
  fEntry++;
 
  fWriteThis=kFALSE; //don't write scaler events on their own, accumulate and write at end or with other events
  RawScaler();
  

  //Now check Event Builder Banks, -2 =>we have all ready got event
  //Note that this funtion will call fEvBank->NextEntry()
  THSHipoReader::ReadEvent(-2); 

  //now other event scalars
  if(fEvBank->GetEntry()<0) return kTRUE;
  fWriteThis=kTRUE; //Got a REC::Event 
  
  fEventInfo->SetTrigBit(fRecEvTRG->Val());
  fEventInfo->SetCJStartTime(fRecEvSTTime->Val());
  fEventInfo->SetRFTime(fRecEvRFTime->Val());
  fEventInfo->SetBeamHel(fRecEvHelic->Val());
  fEventInfo->SetNEvent(fRecEvNEVENT->Val());
  
 
  return kTRUE;

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
