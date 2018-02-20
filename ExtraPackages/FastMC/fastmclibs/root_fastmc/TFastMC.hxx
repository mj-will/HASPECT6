#ifndef TFastMC_HXX
#define TFastMC_HXX
 
#if !defined (__CINT__) || defined (__MAKECINT__)
#include "Rtypes.h"
#endif
 
class TFastMC {
public:
 
  // static void clas_at12g(const Int_t &id,const Float_t &p,const Float_t &thetad, const Float_t &phis, const Float_t &torcur,const Float_t &d_phi,const Float_t &weight, const Int_t &iconf,const Int_t &iacc);
  static void clas_at12g(const Int_t &id,const Float_t &p,const Float_t &thetad, const Float_t &phis, const Float_t &torcur,const Float_t &d_phi,const Float_t &weight, const Char_t cfile[]);
  static void testtt(Int_t &it, Float_t &dt);
   ClassDef(TFastMC,0)  // C++ glue code for clas fastmc functions
};
 
#endif /* TFastMC_HXX */
