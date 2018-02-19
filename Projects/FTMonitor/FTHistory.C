 {
   ///////////////////////////////////////////////////////////////////
  ////Set input files via TChain (i.e. can use wildcards etc.)
  TChain chain("History");
  chain.Add(HSin()+"/*.root");
  chain.Add(HSin()+"/*.root");

  
  THSHipoFTMon* mon=new THSHipoFTMon();
  mon->GetFileNumbers(&chain);
  mon->History(&chain,"History.root");
  
 }
