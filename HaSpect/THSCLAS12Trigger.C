#include "THSCLAS12Trigger.h"
#include <TH1.h>
#include <TSpectrum.h>

void THSCLAS12Trigger::FindTimeOffSetFT(TTree* tree){
  //First look for global FT-CLAS12 offset
  TH1F* hisFTCLAS12=new TH1F("ftclas12","ftclas12",10000,-500,500);
  tree->Draw("Particles[0].DeltaTime()-Particles.DeltaTime()>>his(10000,-500,500)","Particles.Detector()<0&&Particles[0].Time()!=0&&Particles.Detector()!=10000","goff");
  TSpectrum s1(1);
  //use TSpectrum to find time difference peak
  Int_t nfound = s1.Search(hisFTCLAS12,1,"",0.50);
  Double_t *xpeaks=0;
  xpeaks = s1.GetPositionX();
  //Use this as FT time shift
  fTimeShiftFT=xpeaks[0];
  cout<<"THSCLAS12Trigger::FindTimeOffSetFT Detected FT offset of "<<fTimeShiftFT<<endl;
  delete  hisFTCLAS12;
}
void THSCLAS12Trigger::FindTimeRFTimePeak(TTree* tree){
  //Now look for a RF-FDe- time peak to align to
  TH1F* his=new TH1F("peaks","peaks",800,100,180);
  tree->Draw("Particles.DeltaTime()-EventInfo.fRFTime>>peaks","Particles.PDG()==11","goff");
  TSpectrum s20(20);
  Int_t nfound = s20.Search(his,0.4,"",0.50);
  Double_t *xpeaks=0;
  xpeaks = s20.GetPositionX();
  fSTimePeak=xpeaks[0];
  cout<<"THSCLAS12Trigger::FindTimeRFTimePeak position "<<fSTimePeak<<endl;
  delete his;
}
void THSCLAS12Trigger::SubtractStartTime(THSParticle* part){
  part->ShiftTime(-fStartTime);
}
void THSCLAS12Trigger::SubtractStartTime(THSParticle* part0,THSParticle* part1){
  Float_t startime=fStartTime;
  part0->ShiftTime(-startime);
  part1->ShiftTime(-startime);
}
void THSCLAS12Trigger::SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2){
  Float_t startime=fStartTime;
  part0->ShiftTime(-startime);
  part1->ShiftTime(-startime);
  part2->ShiftTime(-startime);
}
void THSCLAS12Trigger::SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3){
  Float_t startime=fStartTime;
  part0->ShiftTime(-startime);
  part1->ShiftTime(-startime);
  part2->ShiftTime(-startime);
  part3->ShiftTime(-startime);
}
void THSCLAS12Trigger::SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4){
  Float_t startime=fStartTime;
  part0->ShiftTime(-startime);
  part1->ShiftTime(-startime);
  part2->ShiftTime(-startime);
  part3->ShiftTime(-startime);
  part4->ShiftTime(-startime);
}
void THSCLAS12Trigger::SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5){
  Float_t startime=fStartTime;
  part0->ShiftTime(-startime);
  part1->ShiftTime(-startime);
  part2->ShiftTime(-startime);
  part3->ShiftTime(-startime);
  part4->ShiftTime(-startime);
  part5->ShiftTime(-startime);
}
void THSCLAS12Trigger::SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5,THSParticle* part6){
  Float_t startime=fStartTime;
  part0->ShiftTime(-startime);
  part1->ShiftTime(-startime);
  part2->ShiftTime(-startime);
  part3->ShiftTime(-startime);
  part4->ShiftTime(-startime);
  part5->ShiftTime(-startime);
  part6->ShiftTime(-startime);
}
void THSCLAS12Trigger::SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5,THSParticle* part6,THSParticle* part7){
  Float_t startime=fStartTime;
  part0->ShiftTime(-startime);
  part1->ShiftTime(-startime);
  part2->ShiftTime(-startime);
  part3->ShiftTime(-startime);
  part4->ShiftTime(-startime);
  part5->ShiftTime(-startime);
  part6->ShiftTime(-startime);
  part7->ShiftTime(-startime);
}
void THSCLAS12Trigger::SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5,THSParticle* part6,THSParticle* part7,THSParticle* part8){
  Float_t startime=fStartTime;
  part0->ShiftTime(-startime);
  part1->ShiftTime(-startime);
  part2->ShiftTime(-startime);
  part3->ShiftTime(-startime);
  part4->ShiftTime(-startime);
  part5->ShiftTime(-startime);
  part6->ShiftTime(-startime);
  part7->ShiftTime(-startime);
  part8->ShiftTime(-startime);
}
void THSCLAS12Trigger::SubtractStartTime(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5,THSParticle* part6,THSParticle* part7,THSParticle* part8,THSParticle* part9){
  Float_t startime=fStartTime;
  part0->ShiftTime(-startime);
  part1->ShiftTime(-startime);
  part2->ShiftTime(-startime);
  part3->ShiftTime(-startime);
  part4->ShiftTime(-startime);
  part5->ShiftTime(-startime);
  part6->ShiftTime(-startime);
  part7->ShiftTime(-startime);
  part8->ShiftTime(-startime);
  part9->ShiftTime(-startime);
}
