//--Author      DI Glazier 30/06/2014
//--Rev
//--Update
//--Description
//HASPECT Event Reconstruction
//THSParticle
//Persistant Data structure
//Contains reconstructed information required for data analysis

#include <iostream>
#include "THSParticle.0.h"

ClassImp(THSParticle);

THSParticle::THSParticle() : fP4(), fVertex(), fPDGCode(0), fPDGMass(0), fMeasMass(0) {
}
THSParticle::THSParticle(int code) : fP4(), fVertex(), fPDGCode(0), fPDGMass(0), fMeasMass(0) {
  SetPDGcode(code);
}
THSParticle::THSParticle(TString pdgname) : fP4(), fVertex(), fPDGCode(0), fPDGMass(0), fMeasMass(0) {
  SetPDGcode(TDatabasePDG::Instance()->GetParticle(pdgname)->PdgCode());
}
void THSParticle::Print(Option_t *) const{
  cout<<"Printing THSParticle of type "<<TDatabasePDG::Instance()->GetParticle(fPDGCode)->GetName()<<endl;
  fP4.Print("");
  fVertex.Print("");
}

void THSParticle::CopyParticle(THSParticle* part){
  SetP4(part->P4());
  SetVertex(part->Vertex());
  SetTime(part->Time());
  SetMeasMass(part->MeasMass());
  SetPDGcode(part->PDG());
}
