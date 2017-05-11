#if !defined(HSDATAWRITER)
#define HSDATAWRITER

#include "IUAmpTools/Kinematics.h"

#include "TTree.h"
#include "TFile.h"

#define max_Part 10

class HSDataWriter{

public:
		
  HSDataWriter( const string& outFile );
  ~HSDataWriter();

  void writeEvent(  );
  void write(  );
  int eventCounter() const { return m_eventCounter; }

  void setTree(TTree* tree){m_outTree=tree->CloneTree(0);m_outTree->SetDirectory(m_outFile);m_inTree=tree;}


private:

  TFile* m_outFile=nullptr;
  TTree* m_outTree=nullptr;
  TTree* m_inTree=nullptr;

  int m_eventCounter;


};

#endif
