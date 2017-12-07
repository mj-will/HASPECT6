#include "THSTopology.h"
#include "TDatabasePDG.h"

/////////////////////////////////////////////////////////////////
///Contructor requires particle definition string
///This will be convertor into a vector of codes to check versus the
///detected particles in the event
///If UseChargeID is set the code is just the particle charge
///If not it is the PDG code
///incl is a string to particles that any number can be detected of for an event
///if incl=="ALL" then all particles are allowed inclusive
THSTopology::THSTopology(TString topo,FinalState::VoidFuncs funcE,
			 TString chPID,TString incl){
  //SetIter(std::bind(&THSTopology::TestFunc,this));
  //Set the pointers to the THSFinalState functions
  Exec=funcE;

  
 
  if(chPID.Sizeof()>1)SetChargePID(chPID);
  if(incl.Sizeof()>1)SetInclusive(incl);
  
  //split the string into an array of particles
  TObjArray* OptList = topo.Tokenize(":");

  for(Int_t i=0;i<OptList->GetEntries();i++){
    Int_t pdg=0;
    Int_t charge=0;
    if(((TObjString*)OptList->At(i))->String()==TString("Rootino+")) {pdg=fNoID;charge=1;}
    else if(((TObjString*)OptList->At(i))->String()==TString("Rootino-")) {pdg=-fNoID;charge=-1;}
    else if(((TObjString*)OptList->At(i))->String()==TString("Beam")) {pdg=-22;charge=-1;}
    else if(!TDatabasePDG::Instance()->GetParticle(((TObjString*)OptList->At(i))->String()))Error("THSFinalState::AddTopology","Particle not found = %s",((TObjString*)OptList->At(i))->String().Data());
    else{
      pdg=TDatabasePDG::Instance()->GetParticle(((TObjString*)OptList->At(i))->String())->PdgCode();
      charge=TDatabasePDG::Instance()->GetParticle(pdg)->Charge();
      if(charge!=0)charge=charge/TMath::Abs(charge); //just get sign not mag.
    }
    if(fUseChargePID) fDefinition.push_back(charge);
    else  fDefinition.push_back(pdg);
   }
  
  //sort into order for checking
  std::sort(fDefinition.begin(),fDefinition.begin()+fDefinition.size());
  // cout<<"THSTopology:: Added Topology ";
  // for(UInt_t i=0;i<fDefinition.size();i++)
  //   cout<<fDefinition[i]<<" ";
  // cout<<endl;
  // cout<<"Is ID code based on charge ? "<<fUseChargePID<<" "<<fChargeParts.size()<<" "<<chPID<<endl;
  // cout<<"Is inclusive ? "<<fIsInclusive<<" "<<fIncParts.size()<<" "<<incl<<endl;


  fIter=new THSParticleIter();
}
///////////////////////////////////////////////////////////
///Destructor
THSTopology::~THSTopology(){
  if(fIter) delete fIter;
}

//////////////////////////////////////////////////////////////////////
///Check if the topology itopo exactly matches the current detected one
Bool_t THSTopology::CheckTopo(vector<Int_t> *detTopo){
  //If want to use charge ID convert this topo from PDG to charge
  if(fUseChargePID) TopoToCharge(detTopo);
  
  if(fIsInclusive) return CheckInclusiveTopo(detTopo);
  return CheckExclusiveTopo(detTopo);
}

Bool_t THSTopology::CheckExclusiveTopo(vector<Int_t> *detTopo){
  if(*(detTopo)==fDefinition){
    return kTRUE;
  }
  return kFALSE;
}
//////////////////////////////////////////////////////////////////////
///Check if the topology itopo matches inclusively the current detected one
Bool_t THSTopology::CheckInclusiveTopo(vector<Int_t> *detTopo){


  vector<Int_t>ptypes;

  for(UInt_t ipart=0;ipart<fDefinition.size();ipart++){
    //check if ptypes already includes this type fDefinition[ipart]
    if(std::find(ptypes.begin(), ptypes.end(), fDefinition[ipart]) == ptypes.end()){
      ptypes.push_back(fDefinition[ipart]);//Not there so add it
      //Now see if sufficient number in thisTopo
      Int_t thiscount = std::count (detTopo->begin(), detTopo->end(), fDefinition[ipart]);
      Int_t topocount = std::count (fDefinition.begin(), fDefinition.end(), fDefinition[ipart]);
      if(thiscount<topocount)//Not sufficient of this type, not this topo
	return kFALSE;
      //Check if this is an allowed inclusive particle
      if(fIncParts.size()){//if size=0 assume all particles are allowd inclusive
	if(std::count (fIncParts.begin(), fIncParts.end(), fDefinition[ipart]))
	  continue; //it is so OK
	else  //it is not so need exact match
	  if(thiscount!=topocount)
	    return kFALSE;
      }
    }
  }
  return kTRUE;
}

////////////////////////////////////////////////////////////////
///Look for topologies by charge rather than pdg code
///if the string parts is given with PDG names, only those particle
///types will use charge as code others will still use PDG
///"ALL" means all particles will use charge
void THSTopology::SetChargePID(TString parts){
  fUseChargePID=kTRUE;
  if(parts==TString("ALL")) return;

 
  fChargeParts.clear();
  //if specified which particles can be inclusive
  //save their pdg in fIncParts
  TObjArray* OptList = parts.Tokenize(":");
  for(Int_t i=0;i<OptList->GetEntries();i++){
    Int_t pdg=0;
    Int_t charge=0;
    if(((TObjString*)OptList->At(i))->String()==TString("Rootino+")) {pdg=fNoID;charge=1;}
    else if(((TObjString*)OptList->At(i))->String()==TString("Rootino-")) {pdg=-fNoID;charge=-1;}
    else if(((TObjString*)OptList->At(i))->String()==TString("Beam")) {pdg=-22;charge=-1;}
    else if(!TDatabasePDG::Instance()->GetParticle(((TObjString*)OptList->At(i))->String()))Error("THSFinalState::SetInclusive","Particle not found = %s",((TObjString*)OptList->At(i))->String().Data());
    else{
      pdg=TDatabasePDG::Instance()->GetParticle(((TObjString*)OptList->At(i))->String())->PdgCode();
      //  charge=TDatabasePDG::Instance()->GetParticle(pdg)->Charge();
      // if(charge!=0)charge=charge/TMath::Abs(charge); //just get sign not mag.
    }
    fChargeParts.push_back(pdg);
  }
  std::sort(fChargeParts.begin(),fChargeParts.begin()+fChargeParts.size());
  cout<<"Charge ID particle are  ";
  if(fChargeParts.size()==0)cout <<" ALL ";
  for(UInt_t i=0;i<fChargeParts.size();i++)
    cout<<fChargeParts[i]<<" ";
  cout<<endl;
  
}
////////////////////////////////////////////////////////////////
///Look for inclusive topologies
///if the string parts is given with PDG names, only those particle
///types are allowed to have more than their topology defined number
void THSTopology::SetInclusive(TString parts){
  fIsInclusive=kTRUE;
  if(parts==TString("ALL")) return;
  fIncParts.clear();
  //if specified which particles can be inclusive
  //save their pdg in fIncParts
  TObjArray* OptList = parts.Tokenize(":");
  for(Int_t i=0;i<OptList->GetEntries();i++){
    Int_t pdg=0;
    Int_t charge=0;
    if(((TObjString*)OptList->At(i))->String()==TString("Rootino+")) {pdg=fNoID;charge=1;}
    else if(((TObjString*)OptList->At(i))->String()==TString("Rootino-")) {pdg=-fNoID;charge=-1;}
    else if(((TObjString*)OptList->At(i))->String()==TString("Beam")) {pdg=-22;charge=0;}
    else if(!TDatabasePDG::Instance()->GetParticle(((TObjString*)OptList->At(i))->String()))Error("THSFinalState::SetInclusive","Particle not found = %s",((TObjString*)OptList->At(i))->String().Data());
    else{
      pdg=TDatabasePDG::Instance()->GetParticle(((TObjString*)OptList->At(i))->String())->PdgCode();
    }
     //add this ParticleID if not there already (in case of charge ID)
    if(std::count (fIncParts.begin(), fIncParts.end(), ParticleID(pdg))==0) fIncParts.push_back(ParticleID(pdg));
  }
  std::sort(fIncParts.begin(),fIncParts.begin()+fIncParts.size());
  cout<<"Incusive particle are  ";
  if(fIncParts.size()==0)cout <<" ALL ";
  for(UInt_t i=0;i<fIncParts.size();i++)
    cout<<fIncParts[i]<<" ";
  cout<<endl;
  
}
void THSTopology::Print(){
  cout<<" THSTopology::Print() : "<<fID<<endl;
  if(fAlternative) cout<<"   Alternative Topo = "<<fAlternative->ID()<<endl;
  cout<<"    particles = ";
  for(UInt_t i=0;i<fDefinition.size();i++)
    cout<<fDefinition[i]<<" ";
  cout<<endl;

  if(fIsInclusive){
    cout<<"The following particle are inclusive: "<<endl;
    if(fIncParts.size()){
      for(UInt_t i=0;i<fIncParts.size();i++)
	cout<<fIncParts[i]<<" ";
      cout<<endl;
    }
    else cout<<" ALL "<<endl;
  }
  if(fUseChargePID){
    cout<<"The following particle are identified by charge : "<<endl;
    if(fChargeParts.size()){
      for(UInt_t i=0;i<fChargeParts.size();i++)
	cout<<fChargeParts[i]<<" ";
      cout<<endl;
    }

    
    else cout<<" ALL "<<endl;
  }
  
  fIter->Print();

}
//////////////////////////////////////////////////////////////////////
///Convert particle PDG code to the particle charge
Int_t THSTopology::PDGtoCharge(Int_t pdg){

  Int_t charge=TDatabasePDG::Instance()->GetParticle(pdg)->Charge();
  if(charge!=0)charge=charge/TMath::Abs(charge); //just get sign not mag.
  return charge;
}
//////////////////////////////////////////////////////////////////////
///return PDG code or charge depending on what is configured via fChargeParts
Int_t THSTopology::ParticleID(Int_t pdg){
  if(!fUseChargePID) return pdg;
   //First check the case of no defined PDG code
    //i.e. Rootinos, these are always converted to charge
 
  if(pdg==fNoID){ return 1;}
  if(pdg==-fNoID){ return -1;}
  if(pdg==0){ return 0;}

  //requested to change all Ids to charge
  if(fChargeParts.size()==0)
    return PDGtoCharge(pdg);
  
  //check if this is assigned charge particle and change it
  if(std::count (fChargeParts.begin(), fChargeParts.end(), pdg))
    return PDGtoCharge(pdg);

  //Just use given pdg
  return pdg;
}

//////////////////////////////////////////////////////////////////////
///Convert this topology to charge identification according to fChargeParts
void THSTopology::TopoToCharge(vector<Int_t> *thisTopo){

  for(UInt_t ip=0;ip<thisTopo->size();ip++){
    thisTopo->at(ip)=ParticleID(thisTopo->at(ip));
  }
  
}
