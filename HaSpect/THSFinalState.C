/**
	\class THSFinalState
	
	Class to control particlular analysis
	Users need to create their own specific final state class
	It should operate on 3 classes of data:
	real, mc rec, mc gen
	input should be in form of events extracted
	from trees or text files,...
	
	The main task is to take detected particles and
	convert them into information needed for subsequent
	fitting of observables, e.g TLorentzVectors for AmpTools
	Double_t s for RooFit
	
	@example THSFinalTemp.C
*/



#include "THSFinalState.h"
#include "THSTopology.h"

THSFinalState::THSFinalState(){
  
  fMapPDGtoParticle[2212]=&fVecProtons;
  fMapPDGtoParticle[2112]=&fVecNeutrons;
  fMapPDGtoParticle[211]=&fVecPiPs;
  fMapPDGtoParticle[-211]=&fVecPiMs;
  fMapPDGtoParticle[111]=&fVecPi0s;
  fMapPDGtoParticle[321]=&fVecKPs;
  fMapPDGtoParticle[-321]=&fVecKMs;
  fMapPDGtoParticle[11]=&fVecEls;
  fMapPDGtoParticle[-11]=&fVecPos;
  fMapPDGtoParticle[22]=&fVecGams;
  fMapPDGtoParticle[-22]=&fVecBeams;
  fMapPDGtoParticle[1E6]=&fVecPlus;
  fMapPDGtoParticle[-1E6]=&fVecMinus;
  fMapPDGtoParticle[0]=&fVec0;

}
////////////////////////////////////////////////////////////////////////
///Add a new topology
///requires particle string defining particles to be detected
/// a function intialising the particle iterator
/// a function defining the specific behaviour for this topology
/// a string if want to use charge as identiification
/// a tring if want to look for inclusive topologies
void THSFinalState::AddTopology(TString topo,FinalState::VoidFuncs funcI,
				FinalState::VoidFuncs funcE,TString chPID,TString incl){
  fCurrTopo=new THSTopology(topo,funcE,chPID,incl);
  fCurrTopo->SetID(fNTopo++);
  
  fTopos.push_back(fCurrTopo);
  fCurrIter=nullptr;
  funcI();//call the assigned particle iterator initialiser
  fCurrIter->ConfigureIters();

  if(!fCurrIter)cout<<"THSFinalState::AddTopology warning no Init_Iter working for topology "<<topo<<endl;
 }
/////////////////////////////////////////////////////////////////////////
///Initialse topologies and anything else before analysis starts
void THSFinalState::InitFS(){
  for(UInt_t i0=0;i0<fTopos.size()-1;i0++)
    for(UInt_t i1=i0+1;i1<fTopos.size();i1++)
      if(fTopos[i0]->CheckExclusiveTopo(fTopos[i1]->Definition())){
	fTopos[i0]->SetAlternative(fTopos[i1]);
	break;
      }

  for(UInt_t i=0;i<fTopos.size();i++)
    fTopos[i]->Print();

}
/////////////////////////////////////////////////////////////////////
///Note particles with TruthOnly are reserved for undetected particles
///which may be kept in simulated data, but shouldn't be included
///in final state check
THSTopology* THSFinalState::FindTopology(){
  if(IsPermutating()) return fCurrTopo;//Topology hasn't changed  with permutation

  fThisTopo.clear();

  for(UInt_t ip=0;ip<frDetParts->size();ip++){
    if(!fUseChargePID)fThisTopo.push_back(frDetParts->at(ip)->PDG());
    else{
      fThisTopo.push_back(frDetParts->at(ip)->Charge());
    }
  }
  std::sort(fThisTopo.begin(),fThisTopo.begin()+fThisTopo.size());
  // cout<<"ThisTopology ";
  // for(UInt_t i=0;i<fThisTopo.size();i++)
  //   cout<<fThisTopo[i]<<" ";
  // cout<<endl;

  //now check to see if this topology matches any of our predefines ones
  fCurrTopo=nullptr;
  // cout<<"TOPOOPOP "<<fTopos.size()<<endl;
  for(UInt_t itopo=0;itopo<fTopos.size();itopo++)
    if(fTopos[itopo]->CheckTopo(&fThisTopo)){
      fCurrTopo=fTopos[itopo];
      break;
    }

  fNParts=fThisTopo.size();

  //Init particle vectors, this should only be done once for each event
  //Not for each permuation
  if(fCurrTopo) InitParticles();
  
  return fCurrTopo;
}


///////////////////////////////////////////////////////////////////
///Read in the particles from this input event and
///Get the first combination from the particle iterator
void THSFinalState::InitParticles(){
  if(fIsPermutating0==kTRUE) return;//already Inited
  // cout<<" InitParticles() "<<fCurrTopo<<endl;
  if(!fUseChargePID)
    //Init all particle type vectors
    InitDetParts();
  
  else//Just use the charge for PID (Need to SetUseChargePID())
    InitDetPartsCharge(); 

  CheckParticles(); //Make sure all particle vectors are below limits
  if(!fCurrTopo) return;

  //Get first combination
  PermutateParticles();
}
/////////////////////////////////////////////////////////////////
///Check Npart limits, to get rid of events were there are too many
///particles,
void THSFinalState::CheckParticles(){
  
  map< Int_t , vector< THSParticle* >* >::iterator itmap;
  for (itmap = fMapPDGtoParticle.begin(); itmap != fMapPDGtoParticle.end(); itmap++)
    {
      if(itmap->second->size()>fMaxPart) {fCurrTopo=nullptr;return;}
    }  
 
}

///////////////////////////////////////////////////////////////////
///Read in the particles from this input event
///Place detected particles in array associated with their pdg type
///Loop over detected particles
void THSFinalState::InitDetParts(){
  //parts->clear();

  //Clear all particle vectors
  map< Int_t , vector< THSParticle* >* >::iterator itmap;
  for (itmap = fMapPDGtoParticle.begin(); itmap != fMapPDGtoParticle.end(); itmap++)
    {
      itmap->second->clear();
    }  
  for(UInt_t ip=0;ip<fNParts;ip++){
    THSParticle* part=frDetParts->at(ip);
    fMapPDGtoParticle[part->PDG()]->push_back(part);
   }
 
}
//////////////////////////////////////////////////////////////
///Read in the particles from this input event
///assign them in vectors corrsponing to their charge
void THSFinalState::InitDetPartsCharge(){

  fMapPDGtoParticle[Int_t(1E6)]->clear();
  fMapPDGtoParticle[Int_t(-1E6)]->clear();
  fMapPDGtoParticle[Int_t(0)]->clear();

  for(UInt_t ip=0;ip<fNParts;ip++){
    THSParticle* part=frDetParts->at(ip);
    fMapPDGtoParticle[Int_t(part->Charge()*1E6)]->push_back(part);
   }
 }

//////////////////////////////////////////////////////////////////
///Process one event from the input tree
void THSFinalState::ProcessEvent(){
 //Process one input event
  InitEvent();
  if(frDetParts)fNDet=frDetParts->size();
  // Int_t firstuse=1;

  while(FSProcess());
  
  FinaliseEvent();
}
Bool_t THSFinalState::FSProcess(){
  
  if(!WorkOnEvent()) return kFALSE;

  
  if(IsGoodEvent()){
    UserProcess(); 
  }

  PermutateParticles();
  if(IsPermutating())
    return kTRUE;
  
  return kFALSE;
}

void THSFinalState::UserProcess(){
  if(fFinalTree) fFinalTree->Fill(); //fill for first combination
  fUID++;
  
}

Bool_t THSFinalState::WorkOnEvent(){
  //Should this event be saved?
  THSFinalState::fGoodEvent=kTRUE;
  THSFinalState::fCorrect=0; //Correct permutation? used for simulation only
  
  //If generated MC events
  Init_Generated();
  if(!THSFinalState::fIsGenerated){
    //Look for reconstructed events
    //if reconstructed Find the detected particles in this event
    if(FindTopology()==nullptr) {fGoodEvent=kFALSE;return fIsPermutating0=kFALSE;}
    //Found a topology execute its Topo function
    fCurrTopo->Exec();
    THSTopology* OrigTopo=fCurrTopo;
    //if that topology failed try an alternative if one is linked to this one
    while(!fGoodEvent&&fCurrTopo->Alternative()){
      fGoodEvent=kTRUE;
      fCurrTopo=fCurrTopo->Alternative();
      fCurrTopo->Exec();
    }
    fTopoID=fCurrTopo->ID();
    fCurrTopo=OrigTopo;
  }
  
  //Calc kinematics
  Kinematics();
  
  //Check if assigned vectors agree with true generated
  //Simulation only
  THSFinalState::CheckTruth();

  if(fIsGenerated) return kTRUE; //Generated only 1 permutation
  return kTRUE;
}

//////////////////////////////////////////////////////////////
///returns true if another valid permuation to be tried \n
///returns false when time to move on \n
///Check through particle types,
///if more than 1 of a type permutate through all combinations
Bool_t THSFinalState::PermutateParticles(){
  if(!fTryPerm&&fNPerm>0) return kFALSE; //not permutating in this analysis
  if(fIsGenerated) return kFALSE; //Truth generated analysis
  if(!fCurrTopo) return kFALSE; //no valid topology
  fNPerm++;
  if(fIsPermutating1) return kTRUE; //so user can do their own permutation
  fIsPermutating0=kTRUE; //Will be set to false when event over
  if(fCurrTopo->Iter()->NextCombitorial()){fCurrTopo->Iter()->SortEvent();return kTRUE;}
  else fCurrTopo->Iter()->SortEvent();
  fIsPermutating0=kFALSE;
  return kFALSE; 
}

///////////////////////////////////////////////////////
///Loop through generated and record momentum distance
void THSFinalState::MatchWithGen(THSParticle *part){
  UInt_t match=0;
  Double_t mindist=1E10;
  for(UInt_t ip=0;ip<frGenParts->size();ip++){
    THSParticle* gen=frGenParts->at(ip);
    Double_t dist=part->p3Distance(gen->P4p()->Vect());
    if(dist<mindist){
      mindist=dist;
      match=ip;
    }
  }
  part->SetTruth(frGenParts->at(match)->P4(),frGenParts->at(match)->Vertex(),frGenParts->at(match)->PDG());
}

//////////////////////////////////////////////////////////////////////
///Loop through generated and record momentum distance 
///if the nearest generated track is the same as the TRUTH return true
Bool_t THSFinalState::IsCorrectTruth(THSParticle *part){
  if(!frGenParts) return kFALSE;
  if(!frGenParts->size())return kFALSE;
  
  UInt_t match=0;
  Double_t mindist=1E10;
  for(UInt_t ip=0;ip<frGenParts->size();ip++){
    THSParticle* gen=frGenParts->at(ip);
    Double_t dist=part->p3Distance(gen->P4p()->Vect());
    if(dist<mindist){
      mindist=dist;
      match=ip;
    }
  }

  if(part->TruthP4()==frGenParts->at(match)->P4()&&part->TruthPDG()==frGenParts->at(match)->PDG())
    return kTRUE;
  return kFALSE;
}

///////////////////////////////////////////////
///Check if our final vectors match with truth
void THSFinalState::CheckTruth(){
  if(fIsGenerated) return;
  //Already got one for this event
  //If we set this correct it would be double counting
  //This can happen when using inclusive topologies
  //And rotate particles that are not in the defined topology
  fCorrect=0;
  if(fGotCorrectOne) return;
  //Check if our final vectors match with truth
  fCorrect=1;
  for(UInt_t ip=0;ip<fFinal.size();ip++)
    fCorrect*=IsCorrectTruth(fFinal[ip]);
  
  if(fCorrect) fGotCorrectOne=kTRUE;
}

//////////////////////////////////////////////////
///Interface to THSParticleIter
///Add selected iterator to previously selected particles
THSParticleIter* THSFinalState::AddSelectToSelected(THSParticleIter* diter,Int_t ni,Int_t nsel,THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5,THSParticle* part6,THSParticle* part7,THSParticle* part8,THSParticle* part9){
  if(nsel*ni>diter->GetNSel()) cout<<"WARNING THSFinalState::AddSelectToSelected : trying to select more particles than exist in original iterator "<<nsel*ni<<" "<< diter->GetNSel()<<endl;
  THSParticleIter *new_iter=AddSelectXofY(ni,nsel,part0,part1,part2,part3,part4,part5,part6,part7,part8,part9);
  diter->SetSelIter(new_iter);
  return new_iter;
}
//////////////////////////////////////////////////
///Interface to THSParticleIter
///Add selected iterator to remaining particles
THSParticleIter* THSFinalState::AddSelectToRemainder(THSParticleIter* diter,Int_t ni,Int_t nsel,THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5,THSParticle* part6,THSParticle* part7,THSParticle* part8,THSParticle* part9){
  THSParticleIter *new_iter=AddSelectXofY(ni,nsel,part0,part1,part2,part3,part4,part5,part6,part7,part8,part9);
  diter->SetRemIter(new_iter);
  return new_iter;
}
//////////////////////////////////////////////////
///Interface to THSParticleIter
///Add selected iterator
THSParticleIter* THSFinalState::AddSelectXofY(Int_t ni,Int_t nsel,THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5,THSParticle* part6,THSParticle* part7,THSParticle* part8,THSParticle* part9){
  
  THSParticleIter *new_iter= new THSParticleIter();
  new_iter->AddEventParticles(part0,part1,part2,part3,part4,part5,part6,part7,part8,part9);
  new_iter->SelectXofY(ni,nsel);
  return new_iter;
}
//////////////////////////////////////////////////
///Interface to THSParticleIter
///Add permutate iterator to previously selected particles
THSParticleIter* THSFinalState::AddPermutateToSelected(THSParticleIter* diter,THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5,THSParticle* part6,THSParticle* part7,THSParticle* part8,THSParticle* part9){ 
  THSParticleIter *new_iter=AddPermutate(part0,part1,part2,part3,part4,part5,part6,part7,part8,part9);
  diter->SetSelIter(new_iter);
  return new_iter;
}
/////////////////////////////////////////////////////////////////
///Interface to THSParticleIter
///Add permutate iterator to remaining particles
THSParticleIter* THSFinalState::AddPermutateToRemainder(THSParticleIter* diter,THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5,THSParticle* part6,THSParticle* part7,THSParticle* part8,THSParticle* part9){
  THSParticleIter *new_iter=AddPermutate(part0,part1,part2,part3,part4,part5,part6,part7,part8,part9);
  diter->SetRemIter(new_iter);
  return new_iter;
}
/////////////////////////////////////////////////////////////////
///Interface to THSParticleIter
///Add selected iterator 
THSParticleIter* THSFinalState::AddPermutate(THSParticle* part0,THSParticle* part1,THSParticle* part2,THSParticle* part3,THSParticle* part4,THSParticle* part5,THSParticle* part6,THSParticle* part7,THSParticle* part8,THSParticle* part9){
  
  THSParticleIter *new_iter= new THSParticleIter();
  new_iter->SetCombi(THSPermutation());
  new_iter->AddEventParticles(part0,part1,part2,part3,part4,part5,part6,part7,part8,part9);
  return new_iter;
}

////////////////////////////////////////////////////////////////
///Interface to THSParticleIter
///Create particle iterator for a particticular species
///Given by the particle vector parts (e.g. fVecMinus)
THSParticleIter* THSFinalState::CreateParticleIter(vector<THSParticle*> *parts,Int_t Nsel){
  THSParticleIter *diter=nullptr;
  if(!fCurrIter){
    diter=fCurrTopo->Iter();
    fCurrIter=diter;
  }
  else{
    diter= new THSParticleIter();
    fCurrIter->SetNextInnerIter(diter);
  }
  
  diter->SetCombi(THSSelection());
  diter->SetParticles(parts);
  diter->SetNSel(Nsel);
  
  return diter;
}
