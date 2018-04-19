#include <sstream>
#include <string>
#include "THSHipoReader.h"
#include <TSystem.h>

THSHipoReader::THSHipoReader(){
  fHipo = new THipo();
  fHipo->SetIsVector(kFALSE);
  //Make vectors to hold pointers to THSParticles
  //These are just used for speed, new THSParticles
  //will be added when required by that event.
  //In the end this vector will contain max number of particles
  //from all the events
  fReadParticles=new vector<THSParticle>;
  
 
}
Bool_t THSHipoReader::Init(TString filename,TString name){
  fCurFileName=gSystem->BaseName(filename);
  cout<<"THSHipoReader::Init "<<fCurFileName<<endl;
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
    fPx=dynamic_cast<THipoItemF*>(fPBank->GetItem("px"));
    fPy=dynamic_cast<THipoItemF*>(fPBank->GetItem("py"));
    fPz=dynamic_cast<THipoItemF*>(fPBank->GetItem("pz"));
    fVx=dynamic_cast<THipoItemF*>(fPBank->GetItem("vx"));
    fVy=dynamic_cast<THipoItemF*>(fPBank->GetItem("vy"));
    fVz=dynamic_cast<THipoItemF*>(fPBank->GetItem("vz"));
    fPid=dynamic_cast<THipoItemI*>(fPBank->GetItem("pid"));
    fBeta=dynamic_cast<THipoItemF*>(fPBank->GetItem("beta"));
    fCharge=dynamic_cast<THipoItemB*>(fPBank->GetItem("charge"));
    //Get the necessary items from REC::Scintillator Bank
    fSBank=fHipo->GetBank("REC::Scintillator");
    fSPindex=dynamic_cast<THipoItemS*>(fSBank->GetItem("pindex"));
    fSTime=dynamic_cast<THipoItemF*>(fSBank->GetItem("time"));
    fSSector=dynamic_cast<THipoItemB*>(fSBank->GetItem("sector"));
    fSEnergy=dynamic_cast<THipoItemF*>(fSBank->GetItem("energy"));
    fSPath=dynamic_cast<THipoItemF*>(fSBank->GetItem("path"));
    fSDet=dynamic_cast<THipoItemB*>(fSBank->GetItem("detector"));
    //Get the necessary items from REC::Calorimeter Bank
    fCalBank=fHipo->GetBank("REC::Calorimeter");
    fCalPindex=dynamic_cast<THipoItemS*>(fCalBank->GetItem("pindex"));
    fCalEnergy=dynamic_cast<THipoItemF*>(fCalBank->GetItem("energy"));
    fCalTime=dynamic_cast<THipoItemF*>(fCalBank->GetItem("time"));
    fCalPath=dynamic_cast<THipoItemF*>(fCalBank->GetItem("path"));
    //Get the necessary items from REC::Cherenkov Bank
    fChBank=fHipo->GetBank("REC::Cherenkov");
    fChPindex=dynamic_cast<THipoItemS*>(fChBank->GetItem("pindex"));
    fChEnergy=dynamic_cast<THipoItemS*>(fChBank->GetItem("nphe"));
    fChDetector=dynamic_cast<THipoItemB*>(fChBank->GetItem("detector"));
    //Get the necessary items from REC::FT Bank
    fFTBank=fHipo->GetBank("REC::ForwardTagger");
    fFTPindex=dynamic_cast<THipoItemS*>(fFTBank->GetItem("pindex"));
    fFTTime=dynamic_cast<THipoItemF*>(fFTBank->GetItem("time"));
    fFTEnergy=dynamic_cast<THipoItemF*>(fFTBank->GetItem("energy"));
    fFTPath=dynamic_cast<THipoItemF*>(fFTBank->GetItem("path"));
    fFTDet=dynamic_cast<THipoItemB*>(fFTBank->GetItem("detector"));
  
    fEvBank=fHipo->GetBank("REC::Event");
    fRecEvSTTime=dynamic_cast<THipoItemF*>(fEvBank->GetItem("STTime"));

  
    if(fAddGenerated&&!fMCBank){
      fMCBank=fHipo->GetBank("MC::Particle");
      fMCPx=dynamic_cast<THipoItemF*>(fMCBank->GetItem("px"));
      fMCPy=dynamic_cast<THipoItemF*>(fMCBank->GetItem("py"));
      fMCPz=dynamic_cast<THipoItemF*>(fMCBank->GetItem("pz"));
      fMCVx=dynamic_cast<THipoItemF*>(fMCBank->GetItem("vx"));
      fMCVy=dynamic_cast<THipoItemF*>(fMCBank->GetItem("vy"));
      fMCVz=dynamic_cast<THipoItemF*>(fMCBank->GetItem("vz"));
      fMCPid=dynamic_cast<THipoItemI*>(fMCBank->GetItem("pid"));
      fReadGenerated=new vector<THSParticle>;
    }
  }
  return kTRUE;
}
void THSHipoReader::CloseReadTree(){
  //noting to do for hipo files (I think)
}
void THSHipoReader::InitOutput(TString filename){
  //CLAS file names ~ out_clas_003311.evio.3.hipo ~out_clas_RUNNO.evio.FILENO.hipo
  //Want the option to write all files into 1 run output file
  //Write all input events on 1 go

  //Initialise
  Int_t run_number=GetRunNumber(filename);
  if(!fCombineFiles) //only merge files if fCombineFiles=kTRUE
    THSDataManager::InitOutput(filename);
  else if(run_number!=fRunNumber){
    TString basen=gSystem->BaseName(filename);
    TString runname(basen(0,16));//out_clas_NNNNNN.
    runname.Append("root");
    runname.Prepend(TString(gSystem->DirName(filename))+"/");
    cout<<"THSHipoReader::WriteParticles Actually write to "<<runname<<endl;
    THSDataManager::InitOutput(runname);
    fRunNumber=run_number;
  }

}
void THSHipoReader::CloseOutput(){
  if(!fCombineFiles){ //only merge files if fCombineFiles=kTRUE
    THSDataManager::CloseOutput();
    return;
  }
  else if(fChainFileN+1<fChainFiles->GetEntries()){
    TString nextfname=fChainFiles->At(fChainFileN+1)->GetTitle();
    if(GetRunNumber(nextfname)!=fRunNumber)//if same run number keep same output open
      THSDataManager::CloseOutput();
  }
  else
    THSDataManager::CloseOutput();//last file
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
    if(!fHipo->NextEvent()) {//end of 1 file
      if(fChainFiles){
	if(!NextChainFile())//check if another file and initilaise
	  return kFALSE; //end of all files
	else fHipo->NextEvent(); //first event of new file
      }
      else return kFALSE; //Only analysing 1 file
    }
    fEntry++;
  }
  if(fEntry%100000==0) cout<<fEntry<<endl;
  fParticles.clear();//reset fParticles
  fGenerated.clear();//reset fParticles
  fPIDs.clear();//reset pid values
  if(fAddGenerated) fGenerated.clear();//reset fGenerated
  if(fAddGenerated&&fMCBank){//First read generated event
    UInt_t NGen=fMCBank->GetEntries();
    Int_t ip=0;
    fGenerated.reserve(NGen);
    while(fMCBank->NextEntry()){
      THSParticle particle;
      particle.SetXYZM(fMCPx->Val(),fMCPy->Val(),fMCPz->Val(),0);
      particle.SetVertex(fMCVx->Val(),fMCVy->Val(),fMCVz->Val());
      particle.SetPDGcode(fMCPid->Val());
      particle.TakePDGMass();
      fGenerated.emplace_back(particle);
    }
    
  }
  //eventbuilder particle bank
  if(fEvBank) fEvBank->NextEntry();
  
  if(fPBank){ //Filling reconstructed fParticles
    UInt_t Nin=fPBank->GetEntries();
    //in case some events have more particles
    // while(Nin>fReadParticles->size()){
    //   fReadParticles->push_back(THSParticle());
    // }
    Int_t ip=0;
    fParticles.reserve(Nin);
    fPIDs.reserve(Nin);
    while(fPBank->NextEntry()){
      THSParticle particle;
      // particle.Clear();
      particle.SetXYZM(fPx->Val(),fPy->Val(),fPz->Val(),0);
      particle.SetVertex(fVx->Val(),fVy->Val(),fVz->Val());
      //if(fBeta->Val())particle->SetMeasMass(particle->P4p()->Rho()/fBeta->Val());
      
      if(!fPid->Val()) particle.SetPDGcode(fCharge->Val()*1E4); //unknown
      else  particle.SetPDGcode(fPid->Val());
       
      //Now look for the associated detector info
      //we must match the detector pindex to the index of this particle entry
      //  cout<<"Done particle "<<endl;
      while(fSPindex->FindEntry(fPBank->GetEntry())){
	//Do something if find a particular detector
	particle.SetTime(fSTime->Val()-fRecEvSTTime->Val());
	particle.SetDeltaE(fSEnergy->Val());
	particle.SetPath(fSPath->Val()/100);
	if(fSDet->Val()>10) //FD
	  particle.SetDetector(1000*fSSector->Val());
	else //CD
	  particle.SetDetector(10000);
      }
      //  cout<<"Done scintialltoer "<<endl;
	
      while(fCalPindex->FindEntry(fPBank->GetEntry())){
	//Do something if find a particular detector
	particle.SetEdep(fCalEnergy->Val()+particle.Edep());
	if(particle.Time()==-(fRecEvSTTime->Val())){
	  particle.SetTime(fCalTime->Val()-fRecEvSTTime->Val());
	  particle.SetPath(fCalPath->Val()/100);
	}
	particle.SetDetector(particle.Detector()+100);
     }
      while(fChPindex->FindEntry(fPBank->GetEntry())){
	//Do something if find a particular detector
	//particle->AddEdep(fChEnergy->Val());
	particle.SetDetector(particle.Detector()+fChDetector->Val());
     }
      // cout<<"Done cherenkoc "<<endl;
	
      while(fFTPindex->FindEntry(fPBank->GetEntry())){
	//Do something if find a particular detector
	particle.SetTime(fFTTime->Val()-fRecEvSTTime->Val());
	particle.SetEdep(fFTEnergy->Val());
	//	particle->SetDeltaE();
	particle.SetPath(fFTPath->Val()/100);
	particle.SetDetector(-1000);
	particle.SetPDGcode(fCharge->Val()*1E4);
	
      }
      fPIDs.emplace_back(particle.PDG());
      fParticles.emplace_back(particle);
   
    }
  }
  return kTRUE;
  
}
