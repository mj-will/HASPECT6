//Datareader class for converting Lund format txt files
//into THSParticles
#ifndef THSHIPOTRIGGER_h
#define THSHIPOTRIGGER_h

#include "THSHipoReader.h"

class THSHipoTrigger: public THSHipoReader{

 public :
  THSHipoTrigger();
  virtual ~THSHipoTrigger(){};

  //  virtual Bool_t Init(TString filename,TString name="");
  virtual Bool_t ReadEvent(Long64_t entry=0);
  virtual void InitOutput(TString filename);

 private :

  //banks
  THipoBank* fFTCalBank=nullptr;
  THipoBank* fFTHodoBank=nullptr;

  //items in banks
  THipoItem* fFTCal_E=nullptr;
  THipoItem* fFTCal_Emax=nullptr;
  THipoItem* fFTCal_Erec=nullptr;
  
  THipoItem* fFTHodo_E=nullptr;

  
  //Trigger variables
  Float_t fFTEsum=0;
  Float_t fFTE_clust=0;
  Float_t fFTEmax_clust=0;
  Float_t fFTErec_clust=0;
 
  Int_t fFT_clust_count = 0;
  Int_t fFT_clust_count_Emax_cond = 0;
  Int_t fFT_clust_count_Erec_cond = 0;

  // FT Hodo: 
  Float_t fFTE_hodo=0;
  Int_t fFT_hodo_count = 0;

  // Trigger:
  Bool_t fPassTrig_Esum=kFALSE;
  Bool_t fPassTrig_cluster_max=kFALSE;
  Bool_t fPassTrig_cluster_rec=kFALSE;
  
 public :
  
 
};

#endif //ifdef THSHIPOTRIGGER
