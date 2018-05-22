//Information required 1 time per run
#ifndef THSRUNINFO_h
#define THSRUNINFO_h

class THSRunInfo{

 public :
  THSRunInfo()=default;
  ~THSRunInfo()=default;
  
 protected :
  Float_t fTotCharge=0; //total gated charge for run
  Float_t fMeanCurrent=0; //average current (needs attenuation factor)
  Int_t fNRun=0;//Run Number
  Short_t fType=0;//Event Type (Data or MC)


 public:
  Int_t NRun(){return fNRun;}
  void SetNRun(Int_t st){fNRun=st;}
  Short_t Type(){return fType;}
  void SetType(Short_t st){fType=st;}
  Float_t TotalCharge(){return fTotCharge;}
  void SetTotalCharge(Float_t st){fTotCharge=st;}
  void SetMeanCurrent(Float_t st){fMeanCurrent=st;}
  Float_t MeanCurrent(){return fMeanCurrent;}

  
};

#endif //
