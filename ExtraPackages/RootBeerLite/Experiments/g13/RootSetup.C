 {
  TString G13=gSystem->Getenv("G13");
  TString ROOTBEER=gSystem->Getenv("ROOTBEER");
  TString ROOTBEER_SLIB=gSystem->Getenv("ROOTBEER_SLIB");
  TString DECAYUTILS=gSystem->Getenv("DECAYUTILS");
  cout<<"*****************"<<DECAYUTILS<<endl;
 
  gSystem->AddIncludePath(TString("-I")+ROOTBEER);
    
  LoadMacro("THSRootBeer.C");

  gSystem->Load(ROOTBEER_SLIB+"/libeloss.so");
  //Linear polarisation
  LoadMacro(G13+"LinearPol.C");

  gSystem->AddIncludePath(TString("-I")+DECAYUTILS);
  
  LoadMacro(DECAYUTILS+"/DecayTrackFunctions.cc");
  LoadMacro(DECAYUTILS+"/DecayTrackCorrections.cc");
  cout<<"*****************"<<DECAYUTILS<<endl;
  LoadMacro(DECAYUTILS+"/DecayFiducialCuts.cc");
 
  // LoadMacro("THSCLASg13.C");

}		
