#ifndef THSCLAS12TRIGGER_h
#define THSCLAS12TRIGGER_h

#include <TTree.h>
#include "THSParticle.h"
#include "THSEventInfo.h"
#include "THSRunInfo.h"
#include <cmath>
#include <vector>

class THSCLAS12Trigger{

 public :
  THSCLAS12Trigger()=default;
  ~THSCLAS12Trigger()=default;


 protected:
  vector<Short_t> fEventSectors=std::vector<Short_t>(11,0);
  vector<THSParticle> *fParticles=nullptr;
  THSEventInfo* fEventInfo=nullptr;
  THSRunInfo* fRunInfo=nullptr;
  Float_t fStartTime=0;
  Float_t fSTimePeak=124.25;
  Float_t fTimeShiftFT=-384.65;
  //Float_t fStartTime=124.25; //GEMC
  //Float_t fSTimePeak=124.25; //GEMC
  //Float_t fTimeShiftFT=0; // GEMC
  
 public:
  Short_t Sector(Int_t det);
  Short_t HitsInCD(){ return fEventSectors[10];}
  Short_t HitsInFT() {return fEventSectors[8];}
  Short_t HitsInFD();
  Short_t HitsInSector(Int_t sect){return fEventSectors[sect];}
  Short_t NFDSectorsHit();
  void EventReset();
  Bool_t TrigStatus(Short_t status);

  void SetParticles(vector<THSParticle> *parts){fParticles=parts;}
  void SetEventInfo(THSEventInfo* info){fEventInfo=info;}
  void SetRunInfo(THSRunInfo* info){fRunInfo=info;if(fRunInfo->Type())SetSim();}
  void SetSim(){fSTimePeak=124.25;fTimeShiftFT=0;};
  
  void ReadParticles();

  Float_t StartTime(Float_t ptime); //calculate event start time
  Float_t StartTime(THSParticle* part); //calculate event start time
  Float_t GetStartTime(){return fStartTime;}

  void FindTimeOffSetFT(TTree* tree,TString option="goff");
  void FindTimeRFTimePeak(TTree* tree,TString option="goff");
  
  void SubtractStartTime(THSParticle* part); //subtract from HSParticle  
  void SubtractStartTime(THSParticle* part0,THSParticle* part1);
  void SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2);
  void SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3);
  void SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4);
  void SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5);
  void SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5,THSParticle* part6);
  void SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5,THSParticle* part6,THSParticle* part7);
  void SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5,THSParticle* part6,THSParticle* part7,THSParticle* part8);
  void SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5,THSParticle* part6,THSParticle* part7,THSParticle* part8,THSParticle* part9);
  
};
inline Float_t THSCLAS12Trigger::StartTime(THSParticle* part){
  //If FT shift time first
  if(part->Detector()<0){part->ShiftTime(fTimeShiftFT);return StartTime(part->DeltaTime());}
  //else
  return StartTime(part->DeltaTime());
}

inline Float_t THSCLAS12Trigger::StartTime(Float_t ptime){
  //supply chosen (e-) particle vertex time
  Float_t rftime=fEventInfo->RFTime();
  //Find the nearest rf beam bucket
  fStartTime=fSTimePeak-4.0080160*((Int_t)(std::round(((fSTimePeak-(ptime-rftime))/4.0080160))))+rftime;
  return fStartTime;
}

inline Short_t THSCLAS12Trigger::Sector(Int_t det){
  //return sector of particle and increment counter
  if(det<0) {fEventSectors[8]++;return 0;} //FT
  Short_t sect=(Short_t)det/1000; //FD 1-6, CD 10, unkown 0
  fEventSectors[sect]++;
  return sect;
}
inline Short_t  THSCLAS12Trigger::HitsInFD(){
  return fEventSectors[1]+			\
    fEventSectors[2]+				\
    fEventSectors[3]+				\
    fEventSectors[4]+				\
    fEventSectors[5]+				\
    fEventSectors[6];
}
inline Short_t  THSCLAS12Trigger::NFDSectorsHit(){
  Short_t nsect=0;
  for(Int_t i=1;i<7;i++) //only the 6 FD sectors
    nsect+=(fEventSectors[i]!=0);
  return nsect;
}
inline void  THSCLAS12Trigger::EventReset(){
for(UInt_t i=0;i<fEventSectors.size();i++)
  fEventSectors[i]=0;
}
Bool_t  THSCLAS12Trigger::TrigStatus(Short_t status){
//Use the EB status to tell what CAL,TOF fired
if(status==4010) return kFALSE; //No CD TOF
if(status==1000) return kTRUE;  //FT
if(status==4100) return kTRUE;  //CD with TOF
if(status>2090&&status<3000) //FD with TOF
  if(status%100)             //and CAL
    return kTRUE;

return kFALSE; //everything else
}
void THSCLAS12Trigger::ReadParticles(){
EventReset();
for(UInt_t i=0;i<fParticles->size();i++)
  if(TrigStatus(fParticles->at(i).Status()))Sector(fParticles->at(i).Detector());
}

#endif
