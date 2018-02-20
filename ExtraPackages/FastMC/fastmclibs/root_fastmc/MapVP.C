gSystem->Load("libEG");
TDatabasePDG* db=new TDatabasePDG();
TNtuple* nt=new TNtuple("Resolution","Resolution","id:p:th:ph:dp:dth:dph:d_phi:weight");
TH2F* hdPvP;
TH2F* hdTHvP;
TH2F* hdPHvP;
void MapVC(Int_t ID,Int_t THETAD0,float TORCUR){
  Double_t ThFudge=2;
  //  Double_t PhFudge0=4;
  //  Double_t PhFudge1=30;
  Double_t PhFudge0=.03*TMath::RadToDeg();
  Double_t PhFudge1=.0*TMath::RadToDeg();
   if(ID==22){
    hdPvP=new TH2F("dPvP","dPvP",200,1,10,200,-5,5);
    hdTHvP=new TH2F("dTHvP","dTHvP",200,1,10,200,-10,10);
    hdPHvP=new TH2F("dPHvP","dPHvP",200,1,10,200,-10,10);
  }
  else{
    hdPvP=new TH2F("dPvP","dPvP",200,1,10,200,-3,3);
    hdTHvP=new TH2F("dTHvP","dTHvP",200,1,10,200,-2,2);
    hdPHvP=new TH2F("dPHvP","dPHvP",200,1,10,200,-5,5);
  }
//void MapVC(Int_t ID,Int_t P0,float TORCUR){
  //Load fastmc interface
  //Need to have conf*.dat in this directory
  gSystem->Load("./libFastMC.so");
  int ID;
  float P,THETAD,PHIS,TORCUR,D_PHI,WEIGHT;
  char* CFILE="conf5.dat";
  // TORCUR=3400;
  float WEIGHT,D_PHI;
  for(Int_t i=0;i<10000000;i++){
    float THETAD1=THETAD0;
    float P0=gRandom->Uniform(0,10);
    float P1=P0;
    // float P1=P0;
    // float THETAD0=gRandom->Uniform(0,100);
    // float THETAD1=THETAD0;
    float PHIS0=gRandom->Uniform(-180,180);
    float PHIS1=PHIS0;
    TFastMC::clas_at12g(ID,P1,THETAD1,PHIS1,TORCUR,D_PHI,WEIGHT,CFILE);
    //Fudge in resolutions
    if(ID!=22){
    //   THETAD1=THETAD0+(THETAD1-THETAD0)*ThFudge;
    //   PHIS1=PHIS0+(PHIS1-PHIS0)*PhFudge0+(PHIS1-PHIS0)*PhFudge1/P0;
    Float_t beta;
    if(db->GetParticle(11)->Mass())  beta=P0/db->GetParticle(11)->Mass();
    else beta=1;
    //  PHIS1=PHIS0+gRandom->Gaus()*sqrt(PhFudge0*PhFudge0+PhFudge1*PhFudge1/P0/P0/beta/beta);
    }
    nt->Fill(ID,P0,THETAD0,PHIS0,P1-P0,THETAD1-THETAD0,PHIS1-PHIS0,D_PHI,WEIGHT);
    if(WEIGHT==1){
      if(ID==22){//gammas
	hdPvP->Fill(P0,(P1-P0)/P0*100);
	hdTHvP->Fill(P0,(THETAD1-THETAD0));
	hdPHvP->Fill(P0,(PHIS1-PHIS0));
      }
      else{//charged
	hdPvP->Fill(P0,(P1-P0)/P0*100);
	hdTHvP->Fill(P0,(THETAD1-THETAD0)*TMath::DegToRad()*1000);
	hdPHvP->Fill(P0,(PHIS1-PHIS0)*TMath::DegToRad()*1000);
      }
    }
  }

  dPvP->FitSlicesY();
  dTHvP->FitSlicesY();
  dPHvP->FitSlicesY();
  TCanvas* can=  new TCanvas("c1_n2", "c1_n2",16,85,1028,550);
   gStyle->SetOptStat(0);
   gStyle->SetOptTitle(0);
  can->SetFillColor(10);
  can->Divide(3,1);
  can->cd(1);
  dPvP_2->GetXaxis()->SetTitle("P (GeV)");
  dPvP_2->GetXaxis()->SetLabelOffset(-0.01);
  dPvP_2->GetXaxis()->SetLabelSize(0.07);
  dPvP_2->GetXaxis()->SetTitleSize(0.07);
  dPvP_2->GetXaxis()->SetTitleOffset(0.65);
  dPvP_2->GetYaxis()->SetLabelSize(0.07);
  dPvP_2->Draw();
  TLatex *   tex = new TLatex(0.2295726,1.03846,"#sigma_{P} (GeV)");
  tex->SetTextSize(0.08639309);
  tex->SetLineWidth(2);
  tex->Draw();
  c1_n2_1->Modified();

  can->cd(2);
  dTHvP_2->GetXaxis()->SetTitle("P (GeV)");
  dTHvP_2->GetXaxis()->SetLabelOffset(-0.01);
  dTHvP_2->GetXaxis()->SetLabelSize(0.07);
  dTHvP_2->GetXaxis()->SetTitleSize(0.07);
  dTHvP_2->GetXaxis()->SetTitleOffset(0.65);
  dTHvP_2->GetYaxis()->SetLabelSize(0.07);
  dTHvP_2->Draw();
  tex = new TLatex(0.2055259,2.097842,"#sigma_{#theta} (mrad)");
  tex->SetTextSize(0.08647527);
  tex->SetLineWidth(2);
  tex->Draw();
  c1_n2_2->Modified();
  can->cd(3);
  dPHvP_2->GetXaxis()->SetTitle("P (GeV)");
  dPHvP_2->GetXaxis()->SetLabelOffset(-0.01);
  dPHvP_2->GetXaxis()->SetLabelSize(0.07);
  dPHvP_2->GetXaxis()->SetTitleSize(0.07);
  dPHvP_2->GetXaxis()->SetTitleOffset(0.65);
  dPHvP_2->GetYaxis()->SetLabelSize(0.07);
  dPHvP_2->Draw();
  tex = new TLatex(0.1945511,0.9343389,"#sigma_{#phi} (mrad)");
  tex->SetTextSize(0.08647527);
  tex->SetLineWidth(2);
  tex->Draw();
  c1_n2_3->Modified();

  c1_n2->cd();
  c1_n2->Modified();
}
