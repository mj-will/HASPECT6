//--Author      DI Glazier 23/03/2018
//--Rev
//--Update
//--Description
//HASPECT Event Reconstruction
//THSParticle
//Optimised Persistant Data structure
//Contains reconstructed information required for data analysis

#ifndef __THSParticle_h__
#define __THSParticle_h__

#include "TLorentzVector.h"
#include "TVector3.h"
#include "TDatabasePDG.h"
#include "TMatrixD.h"
#include <vector>
#include <iostream>
#include <Math/Vector4D.h>
#include <Math/Point3D.h>
#include <Math/DisplacementVector3D.h>
using namespace std;

typedef ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<Double32_t> > HSLorentzVector;
typedef ROOT::Math::PositionVector3D< ROOT::Math::Cartesian3D< Double32_t >, ROOT::Math::DefaultCoordinateSystemTag > HSPosition;

typedef ROOT::Math::DisplacementVector3D< ROOT::Math::Cartesian3D< Double_t >, ROOT::Math::DefaultCoordinateSystemTag > HSMomentum;

#pragma link C++ class vector<THSParticle >+;
#pragma link C++ class vector<THSParticle* >+;

class THSParticle {
 private:
 protected:
  HSLorentzVector fP4;  //4-vector 
  HSLorentzVector fTruthP4;//! true generated 4-vector
  HSPosition fVertex;     //particle vertex position
  HSPosition fTruthV;//! true generated vertex
  Double32_t fPDGMass=0;
  Double32_t fMeasMass=0; //Or other PID info
  Double32_t fTime=0;
  Double32_t fPath=0;
  Double32_t fDoca=0;//!
  Double32_t fEdep=0;
  Double32_t fDeltaE=0;
  Short_t fPDGCode=0;           //PDG number
  Short_t fTruthPDG=0;//! true PDG code
  Short_t fDetector=0; //detector code
 
  //Allow space for covariance matrix
  //The vector will need decoded into the TMatrix for calculations
  vector<Float_t> fCovaMatEntries;//!
  TMatrixD fCovarianceMatrix;//!

  //Note if you want to store these in a file
  //you need to remove the !
  
  
 public:
  THSParticle()=default;  	        //Constructor
  THSParticle(int code);  	        //Constructor
  THSParticle(TString pdgname);  	        //Constructor
  ~THSParticle()=default;	     	//Destructor


  //Setting functions
  void SetPDGcode(Int_t code){
    if(TDatabasePDG::Instance()->GetParticle(fPDGCode=code)){
      TParticlePDG* part=TDatabasePDG::Instance()->GetParticle(fPDGCode=code);
      fPDGMass = part->Mass();}
    else fPDGMass=0;
  }
  void SetP4(HSLorentzVector v){fP4=v;}
  void SetVectPDG(HSLorentzVector v){fP4.SetXYZT(v.X(),v.Y(),v.Z(),sqrt(v.P2()+fPDGMass*fPDGMass));}
  void SetP4(HSLorentzVector *v){fP4=*v;}
  void SetXYZT(Double_t X,Double_t Y,Double_t Z,Double_t T){fP4.SetXYZT(X,Y,Z,T);}
  void SetXYZM(Double_t X,Double_t Y,Double_t Z,Double_t M){fP4.SetXYZT(X,Y,Z,sqrt(M*M+X*X+Y*Y+Z*Z));}
  void SetVertex(HSPosition v){fVertex=v;}
  void SetVertex(Double_t X,Double_t Y,Double_t Z){fVertex.SetXYZ(X,Y,Z);}
  //void SetPol(HSPosition p){fPol=p;}
  //void SetPol(Double_t X,Double_t Y,Double_t Z){fPol.SetXYZ(X,Y,Z);}
  void SetTime(Double_t time){fTime=time;};
  void SetPath(Double_t path){fPath=path;};
  void SetDoca(Double_t doca){fDoca=doca;};
  void SetEdep(Double_t edep){fEdep=edep;};
  void SetDeltaE(Double_t edep){fDeltaE=edep;};
  void SetDetector(Int_t det){fDetector=det;};
  void SetMeasMass(Double_t mass){fMeasMass=mass;};
  void TakePDGMass(){SetVectPDG(fP4);}; //Preserves momentum
  void TakePDGMassFromE(){Double_t rho0=fP4.P();Double_t rho=sqrt(fP4.E()*fP4.E()-fPDGMass*fPDGMass);rho/=rho0;fP4.SetXYZT(fP4.X()*rho,fP4.Y()*rho,fP4.Z()*rho,fP4.E());}; //preserves energy
  // void CreateTruth(){fTruth=new THSParticle();};
  void SetTruth(THSParticle* part){fTruthP4=part->P4();fTruthV=part->Vertex();fTruthPDG=part->PDG();};
  void SetTruth(THSParticle part){fTruthP4=part.P4();fTruthV=part.Vertex();fTruthPDG=part.PDG();};
  void SetTruth(HSLorentzVector part,HSPosition ver,Int_t pdg){fTruthP4=part;fTruthV=ver;fTruthPDG=pdg;};
  //Getting functions
  HSLorentzVector P4(){return fP4;}
  HSLorentzVector* P4p(){return &fP4;}
  HSPosition Vertex(){return fVertex;}
  //HSPosition Pol(){return fPol;}
  Short_t PDG(){return fPDGCode;}
  Double_t PDGMass(){return fPDGMass;}
  Double_t MeasMass(){return fMeasMass;}
  Double_t Time(){return fTime;}
  Double_t MassDiff(){return fPDGMass-fMeasMass;}
  Double_t Edep(){return fEdep;}
  Double_t DeltaE(){return fDeltaE;}
  Double_t Doca(){return fDoca;}
  Double_t Path(){return fPath;}
  Double_t Beta(){return fPath/fTime/2.99792e+08*1E9;}//time ns, path m
  Double_t HypBeta(){Double_t pp=fP4.Rho();return pp/sqrt(pp*pp+fPDGMass*fPDGMass);}
  Double_t HypTime(){return fPath/HypBeta()/2.99792e+08*1E9  ;} //in ns
  //  Double_t DeltaTime(){return HypTime()-fTime;};
  Double_t DeltaTime(){return fTime-HypTime();};
  Double_t DeltaTimeVer(){return DeltaTime()-fVertex.Z()/2.99792e+08*1E9;}
  Int_t Charge();
  Short_t Detector(){return fDetector;}
  
  HSLorentzVector* TruthP4p(){return &fTruthP4;};
  HSLorentzVector TruthP4(){return fTruthP4;};
  HSPosition* TruthVer(){return &fTruthV;};
  Short_t TruthPDG(){return fPDGCode;};
  
 
  void Clear();
  
  void CopyParticle(THSParticle* part,Bool_t andPDG);
  //Utility functions
  virtual void Print(Option_t *option="") const;

  //Add 4-vectors, doca vertices, fix pdg(optional)
  void Add(THSParticle *hsp1, THSParticle *hsp2,Int_t pdg=0);

  //DOCA routines
  /* Double_t MakeVirtualVertex(THSParticle* p1,THSParticle *p2); */
  /* Double_t Calc_dtfDOCA( HSPosition locVertex1, HSPosition locUnitDir1, HSPosition locVertex2, HSPosition locUnitDir2, HSPosition *result); */
  /* void Calc_dtfInterDOCA( HSPosition locUnitDir1, HSPosition locUnitDir2, HSPosition locVertex1,  HSPosition locVertex2, HSPosition *locInterDOCA1, HSPosition *locInterDOCA2); */

 public:
  //need to permutate class
  //order in vector based on particle momentum
  //nb comparitive operator cannot work on pointers so vectors need
  //to be filled with object not pointers for this to work
  friend bool operator< ( const THSParticle& lhs, const THSParticle& rhs ){return lhs.fP4.Rho() < rhs.fP4.Rho(); };

  Double_t p3Distance(HSMomentum vec){return (fP4.Vect()-vec).Mag2();}

  Double_t ResTheta(){return fP4.Theta()-fTruthP4.Theta();};
  Double_t ResPhi(){return fP4.Phi()-fTruthP4.Phi();};
  Double_t ResRho(){return fP4.Rho()-fTruthP4.Rho();};
  Double_t ResE(){return fP4.E()-fTruthP4.E();};
  ClassDef(THSParticle,4) //class THSParticle
};
inline Int_t THSParticle::Charge(){
  
  if(fPDGCode==1E4) return 1;
  else if(fPDGCode==-1E4) return -1;
  else if(fPDGCode!=45){
    Int_t charge=TDatabasePDG::Instance()->GetParticle(fPDGCode)->Charge();
    if(charge!=0)charge=(Int_t) charge/TMath::Abs(charge); //just get sign not mag.
    return charge;
  }
  else return 0;
}

//inline bool THSParticle::operator<( const THSParticle& rhs ) {cout<<" "<<rhs.fP4.Rho()<<endl;return fP4.Rho() < rhs.fP4.Rho(); }
//inline bool THSParticle::operator<( const THSParticle& rhs ) {cout<<" "<<rhs.fP4.Rho()<<endl;return this->fP4.Rho() < rhs.fP4.Rho(); }
//inline bool THSParticle::IsFaster( const THSParticle& lhs,const THSParticle& rhs ) {cout<<" "<<rhs.fP4.Rho()<<endl;return lhs.fP4.Rho() < rhs.fP4.Rho(); }
#endif
