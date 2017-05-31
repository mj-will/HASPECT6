#include "THSProject.h"
//ClassImp(THSProject)


Int_t THSProject::AddTopology(TString topo){
   vector<Int_t> detpart;
  //topo should be a pdg particle list seperated by : e.g. "proton:pi+:pi-"
  TObjArray* OptList = topo.Tokenize(":");
  for(Int_t i=0;i<OptList->GetEntries();i++){
    Int_t pdg=0;
    if(((TObjString*)OptList->At(i))->String()==TString("Rootino+")) pdg=1E6;
    else if(((TObjString*)OptList->At(i))->String()==TString("Rootino-")) pdg=-1E6;
    else if(((TObjString*)OptList->At(i))->String()==TString("Beam")) pdg=-22;
    else if(!TDatabasePDG::Instance()->GetParticle(((TObjString*)OptList->At(i))->String()))Error("THSProject::AddTopology","Particle not found = %s",((TObjString*)OptList->At(i))->String().Data());
    else pdg=TDatabasePDG::Instance()->GetParticle(((TObjString*)OptList->At(i))->String())->PdgCode();
    detpart.push_back(pdg);
    //Make list of all possible detected types to optimise particle vectors
    if(std::find(fDetTypes.begin(), fDetTypes.end(), pdg) == fDetTypes.end()) fDetTypes.push_back(pdg);
  }
  //sort into order for checking
  std::sort(detpart.begin(),detpart.begin()+detpart.size());
  cout<<"AddTopology ";
  for(UInt_t i=0;i<detpart.size();i++)
    cout<<detpart[i]<<" ";
  cout<<endl;
  fTopo.push_back(detpart);
  fNTopo++;
  return fNTopo-1;//return index of this topology
}

Int_t THSProject::FindTopology(){
  //Note particles with TruthOnly are reserved for undetected particles
  //which may be kept in simulated data, but shouldn't be included
  //in final state check
  if(IsPermutating()) return fCurrTopo;//Topology hasn't changed  with permutation
  vector<Int_t> thisTopo;
  for(UInt_t ip=0;ip<frDetParts->GetSize();ip++){
    //    if((frDetParts->At(ip).TruthOnly())==kFALSE)thisTopo.push_back(frDetParts->At(ip).PDG());
    thisTopo.push_back(frDetParts->At(ip).PDG());
  }
  std::sort(thisTopo.begin(),thisTopo.begin()+thisTopo.size());
  // cout<<"ThisTopology ";
  // for(UInt_t i=0;i<thisTopo.size();i++)
  //   cout<<thisTopo[i]<<" ";
  // cout<<endl;

  //now check to see if this topology matches any of our predefines ones
  fCurrTopo=-1;
  for(Int_t itopo=0;itopo<fNTopo;itopo++)
    if(thisTopo==fTopo[itopo]){
      fCurrTopo=itopo;
    }
  fNParts=thisTopo.size();
  thisTopo.clear();
  //Init particle vectors, this should only be done once for each event
  //Not for each permuation
  if(fCurrTopo>-1) InitParticles();
  
  return fCurrTopo;
}
Int_t THSProject::FindInclusiveTopology(Int_t incType){
  //Note particles with TruthOnly are reserved for undetected particles
  //which may be kept in simulated data, but shouldn't be included
  //in final state check
  if(IsPermutating()) return fCurrTopo;//Topology hasn't changed  with permutation
  vector<Int_t> thisTopo;
  for(UInt_t ip=0;ip<frDetParts->GetSize();ip++){
    thisTopo.push_back(frDetParts->At(ip).PDG());
  }
  std::sort(thisTopo.begin(),thisTopo.begin()+thisTopo.size());
 
  //now check to see if this topology matches any of our predefines ones
  fCurrTopo=-1;
  for(Int_t itopo=0;itopo<fNTopo;itopo++){
    vector<Int_t>ptypes;
    Bool_t isThis=kTRUE;//assume it is this topo
    for(UInt_t ipart=0;ipart<fTopo[itopo].size();ipart++){
      //check if ptypes already includes this type fTopo[itopo][ipart]
      if(std::find(ptypes.begin(), ptypes.end(), fTopo[itopo][ipart]) == ptypes.end()){
	ptypes.push_back(fTopo[itopo][ipart]);//Not there so add it
	//Now see if sufficient number in thisTopo
	Int_t thiscount = std::count (thisTopo.begin(), thisTopo.end(), fTopo[itopo][ipart]);
	Int_t topocount = std::count (fTopo[itopo].begin(), fTopo[itopo].end(), fTopo[itopo][ipart]);
	if(thiscount<topocount)//Not sufficient of this type, not this topo
	  isThis=kFALSE;
      }
    }
    ptypes.clear();
    if(isThis){
      //contains at least the particles we need
      if(incType!=-1E9){ //if requested only inclusive in 1 particle type check that is case
       
	UInt_t thiscount = std::count (thisTopo.begin(), thisTopo.end(), incType);
	UInt_t topocount = std::count (fTopo[itopo].begin(), fTopo[itopo].end(), incType); //how many incTypes in original topo
	if(thiscount-topocount!=thisTopo.size()-fTopo[itopo].size())
	  continue; //not just incType extra particles
      }
      fCurrTopo=itopo;
      break; //don't look for any more    
    }
  }
    
  fNParts=thisTopo.size();
  thisTopo.clear();
  //Init particle vectors, this should only be done once for each event
  //Not for each permuation
  if(fCurrTopo>-1) InitParticles();
  
  return fCurrTopo;
}
void THSProject::InitParticles(){
  //This should be optimised to run 1 loop not number of sepcies loops
  if(fIsPermutating0==kTRUE) return;
  //Init all particle type vectors
  InitDetParts(2212,&fVecProtons);
  InitDetParts(211,&fVecPiPs);
  InitDetParts(-211,&fVecPiMs);
  InitDetParts(321,&fVecKPs);
  InitDetParts(-321,&fVecKMs);
  InitDetParts(111,&fVecPi0s);
  InitDetParts(11,&fVecEls);
  InitDetParts(-11,&fVecPos);
  InitDetParts(22,&fVecGams); 
  InitDetParts(1E6,&fVecPlus); 
  InitDetParts(-1E6,&fVecMinus);
  InitDetParts(0,&fVec0); 
  InitDetParts(-22,&fVecBeams);
  //start rotations from 0
  fNProtTurns=0;
  fNPipTurns=0;
  fNPimTurns=0;
  fNPi0Turns=0;
  fNKpTurns=0;
  fNKmTurns=0;
  fNElTurns=0;
  fNPosTurns=0;
  fNPlusTurns=0;
  fNMinusTurns=0;
  fN0Turns=0;
  fNBeamTurns=0;
  fNGamTurns=0;
}
void THSProject::InitDetParts(Int_t pdg,vector<THSParticle> *parts){
  //Place detected particles in array associated with their pdg type
  //Loop over detected particles
  parts->clear();
  //Comment out line below as it can block inclusive topologies
  // if(std::find(fDetTypes.begin(), fDetTypes.end(), pdg) == fDetTypes.end())
  //  return; //not looking for this particle in this project
  //  for(THSParticle& part:*frDetParts){
  for(UInt_t ip=0;ip<fNParts;ip++){
    THSParticle& part=frDetParts->At(ip);
    if(part.PDG()==pdg) parts->push_back(part);
  }
  //order particles in momentum, uses THSParticle::operator<
  std::sort(parts->begin(),parts->end());

}
void THSProject::ProcessEvent(){
  //Process one input event
  fNPerm=0;
   do{
     WorkOnEvent();
     if(fFinalTree)
       if(IsGoodEvent())
	 fFinalTree->Fill(); //fill for first combination
     fNPerm++;
   }
   
    while(IsPermutating());
 }
// Bool_t THSProject::PermutateParticles(){
//   if(!fTryPerm) return kFALSE;
//   //returns true if another valid permuation to be tried
//   //returns false when time to move on
//   //check through particle types
//   //if more than 1 of a type permutate through all combinations
//   fIsPermutating0=kTRUE; //Will be set to false when event over
//   if(std::next_permutation(fVecProtons.begin(),fVecProtons.end())) return kTRUE;
//   if(std::next_permutation(fVecPiPs.begin(),fVecPiPs.end())) return kTRUE;
//   if(std::next_permutation(fVecPiMs.begin(),fVecPiMs.end())) return kTRUE;
//   if(std::next_permutation(fVecPi0s.begin(),fVecPi0s.end())) return kTRUE;
//   if(std::next_permutation(fVecKPs.begin(),fVecKPs.end())) return kTRUE;
//   if(std::next_permutation(fVecKMs.begin(),fVecKMs.end())) return kTRUE;
//   if(std::next_permutation(fVecEls.begin(),fVecEls.end())) return kTRUE;
//   if(std::next_permutation(fVecPos.begin(),fVecPos.end())) return kTRUE;
//   if(std::next_permutation(fVecGams.begin(),fVecGams.end())) return kTRUE;
//   if(std::next_permutation(fVecPlus.begin(),fVecPlus.end())) return kTRUE;
//   if(std::next_permutation(fVecMinus.begin(),fVecMinus.end())) return kTRUE;
//   if(std::next_permutation(fVec0.begin(),fVec0.end())) return kTRUE;
//   if(fVecBeams.size()>0)cout<<fVecBeams.at(0).P4().E()<<endl;
//   if(std::next_permutation(fVecBeams.begin(),fVecBeams.end())) return kTRUE;
 
//   fIsPermutating0=kFALSE;
//   return kFALSE; 
// }
Bool_t THSProject::PermutateParticles(){
  if(!fTryPerm) return kFALSE;
  //returns true if another valid permuation to be tried
  //returns false when time to move on
  //check through particle types
  //if more than 1 of a type permutate through all combinations
  fIsPermutating0=kTRUE; //Will be set to false when event over
  if(RotatePartVector(&fVecProtons,&fNProtTurns)) return kTRUE;
  if(RotatePartVector(&fVecPiPs,&fNPipTurns)) return kTRUE;
  if(RotatePartVector(&fVecPiMs,&fNPimTurns)) return kTRUE;
  if(RotatePartVector(&fVecPi0s,&fNPi0Turns)) return kTRUE;
  if(RotatePartVector(&fVecKPs,&fNKpTurns)) return kTRUE;
  if(RotatePartVector(&fVecKMs,&fNKmTurns)) return kTRUE;
  if(RotatePartVector(&fVecEls,&fNElTurns)) return kTRUE;
  if(RotatePartVector(&fVecPos,&fNPosTurns)) return kTRUE;
  if(RotatePartVector(&fVecGams,&fNGamTurns)) return kTRUE;
  if(RotatePartVector(&fVecPos,&fNPosTurns)) return kTRUE;
  if(RotatePartVector(&fVecMinus,&fNMinusTurns)) return kTRUE;
  if(RotatePartVector(&fVec0,&fN0Turns)) return kTRUE;
  if(RotatePartVector(&fVecBeams,&fNBeamTurns)) return kTRUE;  
  fIsPermutating0=kFALSE;
  return kFALSE; 
}
Bool_t THSProject::RotatePartVector(vector<THSParticle>* vec,Int_t *Nturns){
  //move through vector of particles return false when all done
  if(vec->empty()) return kFALSE;
  if(vec->size()==UInt_t(*Nturns)+1) {
    std::rotate(vec->begin(),vec->begin()+1,vec->end());//rotate back to start	
    *Nturns=0;
    return kFALSE;
  }//Got to the end, ready to start again
  std::rotate(vec->begin(),vec->begin()+1,vec->end());
  *Nturns=(*Nturns)+1;
  return kTRUE;
}
void THSProject::MatchWithGen(THSParticle *part){
  //Loop through generated and record momentum distance
  UInt_t match=0;
  Double_t mindist=1E10;
  for(UInt_t ip=0;ip<frGenParts->GetSize();ip++){
    THSParticle& gen=frGenParts->At(ip);
    Double_t dist=part->p3Distance(gen.P4p()->Vect());
    if(dist<mindist){
      mindist=dist;
      match=ip;
    }
  }
  part->SetTruth(frGenParts->At(match).P4(),frGenParts->At(match).Vertex(),frGenParts->At(match).PDG());

}
Bool_t THSProject::IsCorrectTruth(THSParticle *part){
  if(!frGenParts) return kFALSE;
  if(!frGenParts->GetSize())return kFALSE;
  //Loop through generated and record momentum distance 
  //if the nearest generated track is the same as the TRUTH return true
  UInt_t match=0;
  Double_t mindist=1E10;
  for(UInt_t ip=0;ip<frGenParts->GetSize();ip++){
    THSParticle& gen=frGenParts->At(ip);
    Double_t dist=part->p3Distance(gen.P4p()->Vect());
    if(dist<mindist){
      mindist=dist;
      match=ip;
    }
  }
  if(part->TruthP4()==frGenParts->At(match).P4()&&part->TruthPDG()==frGenParts->At(match).PDG())
    return kTRUE;
  return kFALSE;
}
void THSProject::CheckTruth(){
  if(fIsGenerated) return;
   //Check if our final vectors match with truth
  fCorrect=1;
  for(UInt_t ip=0;ip<fFinal.size();ip++)
    fCorrect*=IsCorrectTruth(fFinal[ip]);
}
