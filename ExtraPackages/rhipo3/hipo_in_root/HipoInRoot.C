//root $RHIPO/Hipo2Root.C HipoInRoot.C
{
  //Edit this macro to only convert the banks selected with ConfigBank
  THipo Hipo;
  Hipo.ConfigOnlyBank("REC"); //all banks in REC::
  // Hipo.ConfigOnlyBank("REC::Particle"); //or just specific banks
  // Hipo.ConfigOnlyBank("REC::Scintillator"); 

  Hipo.InitFile("YOUR_OWN_HIPO_FILE.hipo");  //hipo file to analyse
  Hipo.ConfigBanks(); //This is only need once, so if you are looping over files make sure this is only done the first time

  ////////////////////////////////////////////////
  //Explicitly get the banks I want to use
  THipoBank* hBRecPart=Hipo.GetBank("REC::Particle");
  THipoBank* hBRecScint=Hipo.GetBank("REC::Scintillator");
  
  //Now I need to link the bank items to THipoItem objects I can use
  THipoItem* hIpx=hBRecPart->GetItem("px");
  THipoItem* hIpy=hBRecPart->GetItem("py");
  THipoItem* hIpz=hBRecPart->GetItem("pz");

  THipoItem* hItime=hBRecScint->GetItem("time");
  THipoItem* hIpindex=hBRecScint->GetItem("pindex");

  //Some histograms
  TFile* outFile=new TFile("output.root","recreate");
  TH1F* hisP=new TH1F("momentum","momentum",120,0,12);
  TH1F* hisTh=new TH1F("theta","theta",90,0,90);
  TH1F* hisTime=new TH1F("time","time",200,0,200);
  TH2F* hisPVTime=new TH2F("PVTime","PVTime",50,0,200,50,0,12);

  TLorentzVector particle(0,0,1,1);
  
  while(Hipo.NextEvent()){//loop over events
    //Loop over all the particles for this event and histogram
    Int_t pentry=0;
    while((pentry=hBRecPart->NextEntry())){
      particle.SetXYZM(hIpx->Val(),hIpy->Val(),hIpz->Val(),0);
      hisP->Fill(particle.P());
      hisTh->Fill(particle.Theta()*TMath::RadToDeg());
      //find the corresponding entry in the scintillator bank(if there is one)
      //This uses the scintillator pindex item
      while(hIpindex->FindEntry(pentry)){
	hisTime->Fill(hItime->Val());
	hisPVTime->Fill(hItime->Val(),particle.P());
      }
    }
  }
  hisP->Write();
  hisTh->Write();
  hisTime->Write();
  hisPVTime->Write();
  delete outFile;
}
