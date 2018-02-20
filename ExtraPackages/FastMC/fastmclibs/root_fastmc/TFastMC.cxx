#include "TFastMC.hxx"
#include <iostream>
ClassImp(TFastMC)
#if defined __linux
//On linux Fortran wants this, so we give to it!
int xargv=0;
int xargc=0;
void MAIN__() {}
#endif

namespace FastMC {
extern "C" {
 
// The interface to CLAS fastmc FORTRAN function(s).
 
#ifndef WIN32
#define type_of_call
#else /* WIN32 */
#define type_of_call  _stdcall
 
#define clas_at12gev_ CLAS_AT12GEV
 
#endif /* WIN32 */
 
  extern void type_of_call clas_at12g_(const int &id,const float &p,const float &thetad, const float &phis, const float &torcur,const float &d_phi,const float &weight,const  char cfile[30]);
  // extern void type_of_call clas_at12g_(const int &id,const float &p,const float &thetad, const float &phis, const float &torcur,const float &d_phi,const float &weight,const  int &iconf,const int &iacc);
  extern void type_of_call testtt_(const Int_t &it,const  Float_t &dt);
} /* end of extern "C" */
} /* end of namespace FastMC */
 
// The glue function(s) implementation
 
void TFastMC::clas_at12g(const Int_t &id,const Float_t &p,const Float_t &thetad, const Float_t &phis, const Float_t &torcur,const Float_t &d_phi,const Float_t &weight, const Char_t cfile[30])
{
  //std::cout<<id<<" "<<p<<" "<<thetad<<" "<<phis<<" "<<torcur<<" "<<d_phi<<" "<<weight<<" "<<iconf<<" "<<iacc<<" "<<std::endl;
  FastMC::clas_at12g_(id,p,thetad,phis,torcur,d_phi,weight,cfile); // calls the fastmc clas_at12gev function
}
// void TFastMC::clas_at12g(const Int_t &id,const Float_t &p,const Float_t &thetad, const Float_t &phis, const Float_t &torcur,const Float_t &d_phi,const Float_t &weight, const Int_t &iconf,const Int_t &iacc)
// {
//   //std::cout<<id<<" "<<p<<" "<<thetad<<" "<<phis<<" "<<torcur<<" "<<d_phi<<" "<<weight<<" "<<iconf<<" "<<iacc<<" "<<std::endl;
//   FastMC::clas_at12g_(id,p,thetad,phis,torcur,d_phi,weight,iconf,iacc); // calls the fastmc clas_at12gev function
// }
void TFastMC::testtt(Int_t &it, Float_t &dt){
 
  FastMC::testtt_(it,dt);
  std::cout<<"it is " <<it<<" "<<dt<<std::endl;
}
