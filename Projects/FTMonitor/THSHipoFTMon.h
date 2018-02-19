//Datareader class for converting Lund format txt files
//into THSParticles
#ifndef THSHIPOFTMON_h
#define THSHIPOFTMON_h

#include <TH1.h>
#include <TH2.h>
#include <TGraph.h>
#include "THSHipoTrigger.h"

class THSHipoFTMon: public THSHipoTrigger{

 public :
  THSHipoFTMon();
  virtual ~THSHipoFTMon(){};

  //  virtual Bool_t Init(TString filename,TString name="");
  virtual Bool_t ReadEvent(Long64_t entry=0);
  virtual void InitOutput(TString filename);
  virtual void CloseOutput();
  virtual Bool_t Init(TString filename,TString name);
  virtual void PostWrite();
  virtual void Export2PDF();

  void DeclareHistograms();
  void FillHistograms();
  void Declare_FT_Histograms();
  void Fill_FT_Histograms();
  void Declare_RECFT_Histograms();
  void Fill_RECFT_Histograms();
  void Declare_Particle_Histograms();
  void Fill_Particle_Histograms();


  TH1F* Histogram1D(TString name){return ((TH1F*) fHistograms->FindObject(name));};
  TH2F* Histogram2D(TString name){return ((TH2F*) fHistograms->FindObject(name));};


  Int_t GetSingleFTParticle();
  Int_t GetSingleCDParticle();
  Int_t GetSinglePim();
  Int_t GetSinglePip();
  void ScaleHistsByCharge();
  void SaveSummaryData();
  
 private :

  TList* fHistograms=nullptr;

  //Addtional Banks
  
   THipoBank* fFTParticlesBank=nullptr;
   THipoBank* fFTCALClustBank=nullptr;
   THipoBank* fFTHODOClustBank=nullptr;


   THipoItem*   fFTPB_hodoID=nullptr;
   THipoItem*  fFTPB_calID=nullptr;
   THipoItem*  fFTPB_ch=nullptr;
   THipoItem*  fFTPB_cx=nullptr;
   THipoItem*   fFTPB_cy=nullptr;
   THipoItem*  fFTPB_cz=nullptr;
   THipoItem*  fFTPB_e=nullptr;
   THipoItem*  fFTHODOClust_id=nullptr;
   THipoItem*  fFTHODOClust_x=nullptr;
   THipoItem*  fFTHODOClust_y=nullptr;
   THipoItem*  fFTHODOClust_z=nullptr;
   THipoItem*  fFTHODOClust_e=nullptr;
   THipoItem*  fFTHODOClust_t=nullptr;
   THipoItem*  fFTHODOClust_size=nullptr;
   THipoItem*  fFTCALClust_id=nullptr;
   THipoItem*  fFTCALClust_x=nullptr;
   THipoItem*  fFTCALClust_y=nullptr;
   THipoItem*  fFTCALClust_e=nullptr;
   THipoItem*  fFTCALClust_t=nullptr;
   THipoItem*  fFTCALClust_size=nullptr;
   THipoItem*  fFTREC_e=nullptr;
   THipoItem*  fFTREC_t=nullptr;

   vector<TString> fFileNumbers;

 public :

   void History(TChain* chain,TString outname);
   void AddHistPoint(TH1F* his,Int_t i,Float_t val);
   void GetFileNumbers(TChain* chain);
   

};

#endif //ifdef THSHIPOTRIGGER
