
#include <cassert>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <TROOT.h>
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"
#include "TLorentzVector.h"
#include "IUAmpTools/Kinematics.h"
#include "HSDataReader.h"
#include "TSystem.h"
#include "TROOT.h"

using namespace std;

HSDataReader::HSDataReader( const vector< string >& args ) :
UserDataReader< HSDataReader >(args), 
m_eventCounter( 0 ){
  
  assert(args.size() == 1 || args.size() == 2);
  string inFileName;
  if(args.size() == 2)inFileName=args[1]; //root file name given explicitly, useful for looping over bins
  string configFileName(args[0]);
  
  TH1::AddDirectory( kFALSE );
   gSystem->Load( "libTree" );

  ifstream configFile(configFileName.c_str() );
  if(!configFile.is_open()) {cout<<"HSDataReader Error no file "<<configFileName<<endl; exit(0);};
  vector< string > clines;
  string line;
  string wfilename(""),wname(""),wspecies("");
  string sbeam_energy;
  while (!configFile.eof()){
    configFile>>line;
    cout<<line<<endl;
    if(line==string("Weights:"))configFile>>wfilename>>wname>>wspecies;
    else if(line==string("Beam:")) configFile>>sbeam_energy; 
    else  clines.push_back(line);
  }
  if(args.size() == 1)inFileName=clines[0];//Use root file name given in config file
  string inTreeName(clines[1]);
  string tr(clines[2]);
  string amp_pid(clines[3]);
  cout<<"Opening file "<<inFileName<<" to get tree "<<inTreeName<<" will use the tr_ branch? "<<tr<<" and will give amplitudes the particles "<<amp_pid<<endl;
  
  ifstream fileexists( inFileName.c_str() );
  //create datamamanger and link input tree to m_Particles
  if (fileexists){
    m_data=new THSDataManager();
    m_data->InitTreeReader(inFileName.c_str(),inTreeName.c_str());
    //connect to THSParticles
    m_data->ReadEvent(0);
    m_Particles=m_data->GetReaderParticles();
  }
  else{
    cout << "HSDataReader WARNING:  Cannot find file... " << inFileName << endl;
    exit(0);
  }
  
  if(tr==string("True"))
    m_useTruth=1;
  vector<string> conf_pid;
  const char *str=amp_pid.data();  
  char split=',';
  do
    {
      const char *begin = str;
      while(*str != split && *str)
	str++;
      conf_pid.push_back(string(begin, str));
    } while (0 != *str++);
  
  for(int ip=0;ip<conf_pid.size();ip++)
    cout<<conf_pid[ip]<<endl;
  //cout<<n_part<<" "<<b_particle_id<<" "<<b_n_part<<endl;
 
  m_pid_idx=new int[conf_pid.size()];
  m_namp_pid=0;
  //Loop over and save PIDs given in input config file
  //Find where they are in input particle array
  //Note there can be more particles in the input particle array
  //For example isobars, they will be ignored here
  //The ordering in this input configuration should match the indices
  //in the amplitude class
  
  for(int ia=0;ia<conf_pid.size();ia++){
    int pid_ia=std::atoi(conf_pid[ia].data());
    for(int ib=0;ib<m_Particles->GetSize();ib++)
      if(pid_ia==m_Particles->At(ib).PDG()) {m_pid_idx[m_namp_pid++]=ib;break;}
  }

  for(int ip=0;ip<m_namp_pid;ip++)
    cout<<"Found "<<conf_pid[ip]<<" at "<<m_pid_idx[ip]<< " check "<<m_Particles->At(m_pid_idx[ip]).PDG()<<endl;


  if(wfilename.length()>5){
    cout<<"HSDataReader goind to attempt to open weights file "<<wfilename<<" "<<wfilename.find(".root")<<endl;
    m_data->LoadWeights(wfilename,wname);
    m_WgtSpecies=m_data->GetWeights()->GetSpeciesID(wspecies);
  }
  else {
    cout<<"HSDataReader No Weights file given"<<endl;

  }
  if(sbeam_energy.length()>0){
    double benergy = std::atof(sbeam_energy.c_str());
    //assume kinetic energy
    double ebeam=benergy+0.000510999;
    double pbeam=sqrt(ebeam*ebeam-0.000510999*0.000510999);
    m_beam=TLorentzVector(0,0,pbeam,ebeam);
  }
}


void
HSDataReader::resetSource(){

  m_eventCounter = 0;

}
void
HSDataReader::getTreeEntry(long ev){
  //get entry for writing with HSDataWriter
  getTree()->GetEntry(ev);

}

Kinematics*
HSDataReader::getEvent(){
  while( m_eventCounter < numEvents() ){
    
    m_data->ReadEvent(m_eventCounter);
    
    m_eventCounter++;
    
    vector< TLorentzVector > particleList;
    //if e- beam given 
    if(m_beam.E()>0) particleList.push_back(m_beam);
    for( int i = 0; i < m_namp_pid; ++i ){
      if(!m_useTruth)
	particleList.push_back(m_Particles->At(m_pid_idx[i]).P4());  //push back in order in configuration
      else
	particleList.push_back(m_Particles->At(m_pid_idx[i]).TruthP4());
    }
    double weight=1;
    if(m_WgtSpecies>-1){
      m_data->GetWeightEvent();
      
      weight=m_data->GetWeight(m_WgtSpecies);//get event for selected species
      if(weight) return new Kinematics( particleList,weight );
      //else go through loop again until we get one with a weight
    }
    else return new Kinematics( particleList,weight );
  }
  //while condition never met
  return NULL;
  
}

unsigned int
HSDataReader::numEvents() const{
  if (!m_data) return 0;
  return static_cast< unsigned int >( m_data->GetReadTree()->GetEntries() );
}


