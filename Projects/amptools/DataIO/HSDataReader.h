#if !defined(HSDATAREADER)
#define HSDATAREADER

#include <string>
#include <TROOT.h>
#include "TString.h"
#include "TFile.h"
#include <TTreeReaderArray.h>
#include "TTree.h"
#include <TChain.h>
#include "IUAmpTools/DataReader.h"
#include "IUAmpTools/UserDataReader.h"
#include "THSDataManager.h"

#define max_Part 10

using namespace std;

class HSDataReader : public UserDataReader< HSDataReader >{

public:

  HSDataReader() : UserDataReader< HSDataReader >() { }

  HSDataReader( const vector< string >& args );

  ~HSDataReader(){if(m_data) delete m_data;};
  string name() const { return "HSDataReader"; }

  virtual Kinematics* getEvent();
  virtual void resetSource();

  virtual unsigned int numEvents() const;
  int eventCounter() const { return m_eventCounter; }

  TTree* getTree(){return m_data->GetReadTree();}
  void  getTreeEntry(long ev);

private:

  THSDataManager* m_data=nullptr;
  TTreeReaderArray<THSParticle> *m_Particles=nullptr;


  int m_eventCounter;
  int m_namp_pid;
  int* m_pid_idx;

  int m_WgtSpecies=-1;
  bool m_useTruth=0;
  TLorentzVector m_beam; //for electron beam
  
};

#endif
