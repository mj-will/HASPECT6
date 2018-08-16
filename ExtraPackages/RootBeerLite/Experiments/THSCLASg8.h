//Datareader class for interacing to CLAS DST and BOS files with RootBeer
//into THSParticles
#ifndef THSCLASG8_h
#define THSCLASG8_h

#include "THSRootBeer.h"
#include <map>
#define PARA 0                  //defs for polarisation plane
#define PERP 1
#define AMO  2
#define CHANGING 3
#define ALUMINUM 0               //defs for radiators
#define AMORPHOUS 1
#define DIAMOND 2
#define UNKNOWN 3
#define MANUAL 0                //defs for kind of running
#define SWAP 1

//enumerator for the column names in the pol tables
enum {
  E_ID    = 0,
  ENERGY  = 1,
  ENH     = 2,
  ENHERR  = 3,
  ENHFIT  = 4,
  PFIT    = 5,
  PCOR    = 6,
  PCORERR = 7,
  PSMOOTH = 8,
  EDGE    = 9
};

class THSCLASg8: public THSRootBeer{

 public :
  THSCLASg8();
  ~THSCLASg8()=default;

  virtual Bool_t Init(TString filename,TString name="");
  virtual Bool_t ReadEvent(Long64_t entry=0);
  Double_t HypothesisBeta(Double_t pp,Double_t mm){return pp/sqrt(pp*pp+mm*mm);}
  
 private :
 
  virtual void MakeDetected();
  virtual void MakeParticle(Int_t ip);
  virtual Bool_t MakeBeam(Float_t Tmid,Float_t Tcut);
  void MakeTruth();
  void MakeTruthParticle(Int_t ip);
  Int_t EPICSEvent();
  Int_t fCurrentPlane=0;
  Float_t fCurrentEdge=0;
  Int_t   currentMode=MANUAL;
  Int_t  fEgSetting=1900;
  Double_t  fpolTable[2][500][385][10];  //where its [plane][edge][E_id][field]
  Int_t  fpolTableN[2]={0,0};            //No of entries for para and perp
  Char_t fpolFirstLines[2][500][250];   //to keep the 1st lines if needed (info from files)

  int run;                 //run no, extracted from current file name
  char edgeTable[200];
  Int_t edgeEventLow[5000];            //hold the current table of edge positions for event ranges
  Int_t edgeEventHigh[5000];
  Double_t edgeEventEdge[5000];
  Int_t edgeEventPlane[5000];
  Int_t edgeEventN;
  Int_t edgeIndex=0;
  Long64_t lastEdgeEvent=0;
  Double_t lastCohEdge=0.0;
  Int_t lastCohPlane=-1;

 public :
  Int_t LoadPolTable(int plane,const  Char_t *PolTableList);
  Double_t GetPol(Int_t plane, Double_t edge, Int_t eid, Int_t poltype, Double_t lowThresh, Double_t highThresh);
  Double_t GetPol(Int_t plane, Double_t edge, Double_t eg, Int_t poltype, Double_t lowThresh, Double_t highThresh);
  Int_t LoadEdgeTable(Char_t *EdgeTable);
  Int_t GetEdge(Int_t event);
  Double_t mikeTest(Double_t pol, Double_t egamma, Double_t cohEdge, Int_t Plane, Int_t setting); //Mikes pol correction
  Double_t polFix0(Double_t xE, Int_t plane, Int_t parSet); //Mikes pol correction 
  void LoadPolarisations();
  Float_t GetPol(Int_t iphot);

  void SetEdge(Int_t edge){fEgSetting=edge;}
};

#endif //ifdef THSCLASG8
