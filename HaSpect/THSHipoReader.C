#include <sstream>
#include <string>
#include "THSHipoReader.h"
#include <TSystem.h>

THSHipoReader::THSHipoReader(){
  fHipo = new THipo();

  //Make vectors to hold pointers to THSParticles
  //These are just used for speed, new THSParticles
  //will be added when required by that event.
  //In the end this vector will contain max number of particles
  //from all the events
  fReadParticles=new vector<THSParticle*>;
  
 
}
Bool_t THSHipoReader::Init(TString filename,TString name){
  fHipo->InitFile(filename);
  //File opened can now configure banks
  if(!fPBank){
    Info("THSHipoReader","Opened file");
    fHipo->ConfigOnlyBank("REC::Particle");
    fHipo->ConfigOnlyBank("REC::Scintillator");
    fHipo->ConfigOnlyBank("REC::Calorimeter");
    fHipo->ConfigOnlyBank("REC::Cherenkov");
    fHipo->ConfigOnlyBank("REC::ForwardTagger");
    fHipo->ConfigOnlyBank("REC::Event");
    if(fAddGenerated&&!fMCBank){
      fHipo->ConfigOnlyBank("MC::Particle");
    }
    fHipo->ConfigBanks();
    
    //fHipo->ConfigOnlyBank("CVTRec::Tracks");
    //Get the necessary items from Particle Bank
    fPBank=fHipo->GetBank("REC::Particle");
    fPx=fPBank->GetItem("px");
    fPy=fPBank->GetItem("py");
    fPz=fPBank->GetItem("pz");
    fVx=fPBank->GetItem("vx");
    fVy=fPBank->GetItem("vy");
    fVz=fPBank->GetItem("vz");
    fPid=fPBank->GetItem("pid");
    fBeta=fPBank->GetItem("beta");
    fCharge=fPBank->GetItem("charge");
    //Get the necessary items from REC::Scintillator Bank
    fSBank=fHipo->GetBank("REC::Scintillator");
    fSPindex=fSBank->GetItem("pindex");
    fSTime=fSBank->GetItem("time");
    fSSector=fSBank->GetItem("sector");
    fSEnergy=fSBank->GetItem("energy");
    fSPath=fSBank->GetItem("path");
    fSDet=fSBank->GetItem("detector");
    //Get the necessary items from REC::Calorimeter Bank
    fCalBank=fHipo->GetBank("REC::Calorimeter");
    fCalPindex=fCalBank->GetItem("pindex");
    fCalEnergy=fCalBank->GetItem("energy");
    fCalTime=fCalBank->GetItem("time");
    fCalPath=fCalBank->GetItem("path");
    //Get the necessary items from REC::Cherenkov Bank
    fChBank=fHipo->GetBank("REC::Cherenkov");
    fChPindex=fChBank->GetItem("pindex");
    fChEnergy=fChBank->GetItem("nphe");
    fChDetector=fChBank->GetItem("detector");
    //Get the necessary items from REC::FT Bank
    fFTBank=fHipo->GetBank("REC::ForwardTagger");
    fFTPindex=fFTBank->GetItem("pindex");
    fFTTime=fFTBank->GetItem("time");
    fFTEnergy=fFTBank->GetItem("energy");
    fFTPath=fFTBank->GetItem("path");
    fFTDet=fFTBank->GetItem("detector");
    
    fEvBank=fHipo->GetBank("REC::Event");
    fRecEvSTTime=fEvBank->GetItem("STTime");
    
    
    if(fAddGenerated&&!fMCBank){
      fMCBank=fHipo->GetBank("MC::Particle");
      fMCPx=fMCBank->GetItem("px");
      fMCPy=fMCBank->GetItem("py");
      fMCPz=fMCBank->GetItem("pz");
      fMCVx=fMCBank->GetItem("vx");
      fMCVy=fMCBank->GetItem("vy");
      fMCVz=fMCBank->GetItem("vz");
      fMCPid=fMCBank->GetItem("pid");
      fReadGenerated=new vector<THSParticle*>;
    }
  }
  return kTRUE;
}
void THSHipoReader::CloseReadTree(){
  //noting to do for hipo files (I think)
}
void THSHipoReader::WriteParticles(TString filename){
  //CLAS file names ~ out_clas_003311.evio.3.hipo ~out_clas_RUNNO.evio.FILENO.hipo
  //Want the option to write all files into 1 run output file
  //Write all input events on 1 go

  //Initialise
  Int_t run_number=GetRunNumber(filename);
  if(!fCombineFiles) //only merge files if fCombineFiles=kTRUE
     InitOutput(filename);
  else if(run_number!=fRunNumber){
    TString basen=gSystem->BaseName(filename);
    TString runname(basen(0,16));//out_clas_NNNNNN.
    runname.Append("root");
    runname.Prepend(TString(gSystem->DirName(filename))+"/");
    cout<<"THSHipoReader::WriteParticles Actually write to "<<runname<<endl;
    InitOutput(runname);
    fRunNumber=run_number;
  }

  //Loop over events
  while(ReadEvent()){
    if(fWriteThis&&fWriteTree){fWriteTree->Fill();PostWrite();}
    fWriteThis=kTRUE;
  }

  //Close
  if(!fCombineFiles) //only merge files if fCombineFiles=kTRUE
    CloseOutput();
  else if(fChainFileN+1<fChainFiles->GetEntries()){
    TString nextfname=fChainFiles->At(fChainFileN+1)->GetTitle();
    if(GetRunNumber(nextfname)!=fRunNumber)
      CloseOutput();
  }
  else
    CloseOutput();//last file
}
Int_t THSHipoReader::GetRunNumber(TString filen){
  TString base(gSystem->BaseName(filen));
  TString sNum0(base(base.First("0"),6));
  return sNum0.Atoi();
}


Bool_t THSHipoReader::ReadEvent(Long64_t entry){
  //if entry ==-2 we have been called from a derived class who has
  //already got the event 
  if(entry!=-2){
    if(!fHipo->NextEvent()) return kFALSE;
    fEntry++;
  }
  //  cout<<fEntry<<endl;
  fParticles.clear();//reset fParticles
  if(fAddGenerated) fGenerated.clear();//reset fGenerated
  
  if(fAddGenerated&&fMCBank){//First read generated event
    UInt_t NGen=fMCPid->Size();
    while(NGen>fReadGenerated->size()){//make sure enough space in vector
      fReadGenerated->push_back(new THSParticle());
    }
    Int_t ip=0;
    while(fMCBank->NextEntry()){
      THSParticle* particle=fReadGenerated->at(ip++);
      fGenerated.push_back(particle);
      particle->SetXYZM(fMCPx->Val(),fMCPy->Val(),fMCPz->Val(),0);
      particle->SetVertex(fMCVx->Val(),fMCVy->Val(),fMCVz->Val());
      particle->SetPDGcode(fMCPid->Val());
      particle->TakePDGMass();
    }

  }
  //eventbuilder particle bank
  if(fEvBank) fEvBank->NextEntry();
  
  if(fPBank){ //Filling reconstructed fParticles
    UInt_t Nin=fPid->Size();
    //in case some events have more particles
    while(Nin>fReadParticles->size()){
      fReadParticles->push_back(new THSParticle());
    }
    Int_t ip=0;
    while(fPBank->NextEntry()){
      THSParticle* particle=fReadParticles->at(ip++);
      fParticles.push_back(particle);
      particle->Clear();
      particle->SetXYZM(fPx->Val(),fPy->Val(),fPz->Val(),0);
      particle->SetVertex(fVx->Val(),fVy->Val(),fVz->Val());
      //if(fBeta->Val())particle->SetMeasMass(particle->P4p()->Rho()/fBeta->Val());
      if(!fPid->Val()) particle->SetPDGcode(fCharge->ValI()*1E6); //unknown
      else  particle->SetPDGcode(fPid->ValI());
      //Now look for the associated detector info
      //we must match the detector pindex to the index of this particle entry
      
      while(fSPindex->FindEntry(fPBank->GetEntry())){
	//Do something if find a particular detector
	particle->SetTime(fSTime->Val()-fRecEvSTTime->Val());
	particle->SetDeltaE(fSEnergy->Val());
	particle->SetPath(fSPath->Val()/100);
	if(fSDet->Val()>10) //FD
	  particle->SetDetector(1000*fSSector->Val());
	else //CD
	  particle->SetDetector(10000);
     }
 	
      while(fCalPindex->FindEntry(fPBank->GetEntry())){
	//Do something if find a particular detector
	particle->SetEdep(fCalEnergy->Val()+particle->Edep());
	if(particle->Time()==-(fRecEvSTTime->Val())){
	  particle->SetTime(fCalTime->Val()-fRecEvSTTime->Val());
	  particle->SetPath(fCalPath->Val()/100);
	}
	particle->SetDetector(particle->Detector()+100);
     }
      while(fChPindex->FindEntry(fPBank->GetEntry())){
	//Do something if find a particular detector
	//particle->AddEdep(fChEnergy->Val());
	particle->SetDetector(particle->Detector()+fChDetector->Val());
     }
 	
      while(fFTPindex->FindEntry(fPBank->GetEntry())){
	//Do something if find a particular detector
	particle->SetTime(fFTTime->Val()-fRecEvSTTime->Val());
	particle->SetEdep(fFTEnergy->Val());
	//	particle->SetDeltaE();
	particle->SetPath(fFTPath->Val()/100);
	particle->SetDetector(-1000);
	particle->SetPDGcode(fCharge->ValI()*1E6);
	
      }
      
      
    }
  }

  return kTRUE;
  
}
