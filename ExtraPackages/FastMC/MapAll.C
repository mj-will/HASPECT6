
{
  //Load fastmc interface
  //Need to have conf*.dat in this directory
  int ID;
  float P,THETAD,PHIS,TORCUR,D_PHI,WEIGHT;
  //fotran OPEN does not like the name string to be too long...
  gSystem->Exec("ln -s $FASTMCLIB/conf5.dat conf5.dat");

  TString CFILE="conf5.dat";
  TORCUR=3400;
  TNtuple* nt=new TNtuple("Resolution","Resolution","id:p:th:ph:dp:dth:dph:d_phi:weight");
  int iID[9]={11,211,-211,2212,22,45};
  for(Int_t ipar=0;ipar<6;ipar++)
     for(Float_t ip=1.0;ip<4.1;ip+=0.4)
       for(Float_t ith=10;ith<150;ith+=5)
	for(Float_t iph=3;iph<30;iph+=3){
	  for(Int_t i=0;i<1000;i++){
	    ID=iID[ipar];
	    P=ip;
	    THETAD=ith;
	    PHIS=iph;
	    TFastMC::clas_at12g(ID,P,THETAD,PHIS,TORCUR,D_PHI,WEIGHT,CFILE.Data());
	    nt->Fill(ID,ip,ith,iph,P-ip,THETAD-ith,PHIS-iph,D_PHI,WEIGHT);
	  }
	}
}
