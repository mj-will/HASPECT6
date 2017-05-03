#include "THSPartOutput.h"

THSPartOutput::~THSPartOutput(){
  //clear THSParticles memory
  for(UInt_t i=0;i<fDetParticle.size();i++)
    SafeDelete(fDetParticle[i]);

}
void THSPartOutput::InitOutput(){
  //used the predefined string in fStrParticles to setup output tree branches
 //Make output Tree   
   if(!fOutTree) fOutTree=new TTree("HSParticles","A tree containing reconstructed particles");

  Int_t buff=64000;
  Int_t split=0;//note split is important in the TSelector framework, if increased branches in subsequent selectors will be data members of the THSParticle object rather than the whole object (this can lead to name conflicts)
   
   //now automatically create requested detected particles
   //Get the final state defined in fStrParticles
   fNdet=CountChar(fStrParticles,':')+1; //number of particles is just "number of :" +1
   Int_t pos=0;//marker for position in particle list string
   for(UInt_t ifs=0;ifs<fNdet;ifs++){//loop over fStrParticles string and seperate particles
     TString pName;
     if(fStrParticles.Index(":",pos+1)>0)pName=fStrParticles(pos,fStrParticles.Index(":",pos+1)-pos);//split the fStrParticles string
     else pName=fStrParticles(pos,fStrParticles.Length()-pos);//no more ':' so last particle
     pos=fStrParticles.Index(":",pos+1)+1;//move pos on to the next particle
     fDetID.push_back(TDatabasePDG::Instance()->GetParticle(pName)->PdgCode());//get PDG code for particle
     fFinalState.push_back(fDetID[ifs]); //save ID in fFinalState to compare each event with
   }
   //order the final state vector, allows easy comparison with event vector, this is ascending order
   //!!!!!!!!!!!!!!!!!!!!!!very important step for event algorithm!!!!!!!!!!
   std::sort(fFinalState.begin(),fFinalState.end());
   //sort the number of different particle types into fNtype
   Int_t vipart=0;//counter to move to next particle type in fFinalState
   Int_t nvipart=0;//counter of number of particle types
   while((nvipart=std::count(fFinalState.begin(),fFinalState.end(),fFinalState[vipart]))){fNtype.push_back(nvipart);vipart+=nvipart;}
  //now make the branches and particles
   UInt_t Nbranches=0;
   vipart=0;
   for(UInt_t itype=0;itype<fNtype.size();itype++){//loop over types of particles
     fIDtype.push_back(fFinalState[Nbranches]);//get the pdg id of this type
     for(UInt_t intype=0;intype<fNtype[itype];intype++){//loop over particles of same type
       //create particles, note the vector will look after deleting these when it goes out of scope
       fDetParticle.push_back(new THSParticle(fFinalState[Nbranches]));
       TString pName=TDatabasePDG::Instance()->GetParticle(fFinalState[Nbranches])->GetName();
       pName.ReplaceAll("+","p");  //Get rid of + and - in names so not to confuse tree->Draw
       pName.ReplaceAll("-","m");
       pName+=TString("_")+=intype;//append the number of instance of this particle to PDG name
       fOutTree->Branch(pName,fDetParticle[Nbranches],buff,split); //make the branch
       cout<<"Made branch "<<pName<<" the particle can be accessed in the code with fDetParticle["<<Nbranches<<"]"<<endl;
       Nbranches++;
     }//end intype loop
   }//end itype loop
}
