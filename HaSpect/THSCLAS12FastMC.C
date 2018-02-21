#include "THSCLAS12FastMC.h"
#include <TSystem.h>

Bool_t THSCLAS12FastMC::Init(TString filename,TString name){
  THSLundReader::Init(filename,name);

  gSystem->Exec("ln -s $FASTMCLIB/conf5.dat conf5.dat");

  return kTRUE;
}

Bool_t THSCLAS12FastMC::ReadEvent(Long64_t entry){
  
  Bool_t GotOne=ReadGenerated(entry); //just fill generated branch
  if(!GotOne) return kFALSE;

   while(fNin>fReadParticles->size())
      fReadParticles->push_back(new THSParticle());
   fParticles.clear();//reset fParticles
   
   for(UInt_t i=0;i<  fNin;i++){
     Int_t pdg=fGenerated[i]->PDG();
     if(!(pdg==2212||pdg==211||pdg==-211||pdg==321||pdg==321||pdg==11||pdg==22))
       continue;	    
     TLorentzVector* vec=fGenerated[i]->P4p();	
     Double_t r2d=TMath::RadToDeg();
     Float_t THETAD0=vec->Theta()*r2d;
     Float_t P0=vec->Rho();
     Float_t PHIS0=vec->Phi()*r2d;
     Float_t THETAD1=THETAD0;
     Float_t P1=P0;
     Float_t PHIS1=PHIS0;
     Float_t WEIGHT=0;
     TFastMC::clas_at12g(fGenerated[i]->PDG(),P1,THETAD1,PHIS1,fTORUS,0,WEIGHT,fCFILE.Data());
     if(WEIGHT){
       THSParticle* hs=fReadParticles->at(i);
       TLorentzVector* hs4=hs->P4p();
       fParticles.push_back(hs);
       Double_t Mass=vec->M();
      
       hs4->SetVectM(TVector3(0,1,1),0);
       hs4->SetE(sqrt(P1*P1+Mass*Mass)); 	  
       //hs4->SetE(P1); 	  
       hs4->SetRho(P1);
       hs4->SetPhi(PHIS1*TMath::DegToRad());
       hs4->SetTheta(THETAD1*TMath::DegToRad());
       hs->SetPDGcode(fGenerated[i]->PDG());
       hs->SetTruth(*vec,fGenerated[i]->Vertex(),fGenerated[i]->PDG());
       if(THETAD0<5)fParticles.back()->SetDetector(-100);
       else if(THETAD0>35)fParticles.back()->SetDetector(10000);
       else fParticles.back()->SetDetector(1000);
 
     }
  }
  return kTRUE;
}
