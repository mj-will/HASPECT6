#include "HSDataWriter.h"
#include "TH1.h"
#include "TSystem.h"
#include <iostream>
using namespace std;

HSDataWriter::HSDataWriter( const string& outFile ){

  TH1::AddDirectory( kFALSE );
  
  m_outFile = new TFile( outFile.c_str(), "recreate" );

  m_eventCounter = 0;

}


HSDataWriter::~HSDataWriter(){

  // 
  if(m_outFile->IsOpen())write();
  //delete m_outTree;
  delete m_outFile;
}
void HSDataWriter::write(  ){
  //write tree to file
  // m_outFile->cd();
  if(m_outTree) cout<<"HSDataWriter() writing tree with "<<m_outTree->GetEntries()<<" "<<"events "<<m_outTree->GetName()<<endl;
  m_outFile->cd();
  m_outTree->Write();
  m_outFile->Close();
}

void HSDataWriter::writeEvent(  ){

  m_outTree->Fill();

  m_eventCounter++;

}
