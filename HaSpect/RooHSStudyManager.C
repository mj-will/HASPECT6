#include "RooHSStudyManager.h"
#include "RooFit.h"
#include "Riostream.h"
#include "RooWorkspace.h"
#include "RooAbsStudy.h"
#include "RooDataSet.h"
#include "RooMsgService.h"
#include "RooStudyPackage.h"
#include "TTree.h"
#include "TFile.h"
#include "TRegexp.h"
#include "TKey.h"
#include <string>
#include "TROOT.h"
//#include "TInterpreter.h"
#include "TEnv.h"
#include "TProof.h"
#include "TSystem.h"

using namespace std ;

ClassImp(RooHSStudyManager)
  ;



//_____________________________________________________________________________
RooHSStudyManager::RooHSStudyManager(RooWorkspace& w):RooStudyManager(w)
{  
  //  _pkg = new RooStudyPackage(w) ;
}



//_____________________________________________________________________________
RooHSStudyManager::RooHSStudyManager(RooWorkspace& w, RooAbsStudy& study) :RooStudyManager(w,study)
{  
  //_pkg = new RooStudyPackage(w) ;
  //_pkg->addStudy(study) ;
}


//_____________________________________________________________________________
RooHSStudyManager::RooHSStudyManager(const char* studyPackFileName) :RooStudyManager(studyPackFileName)
{
  // string pwd = gDirectory->GetName() ;
  // TFile *f = new TFile(studyPackFileName) ;
  // _pkg = dynamic_cast<RooStudyPackage*>(f->Get("studypack")) ;
  // gDirectory->cd(Form("%s:",pwd.c_str())) ;
}

void RooHSStudyManager::runMyProof(Int_t nExperiments, const char* proofHost, Bool_t showGui) 
{
  //This derived class is only required to implement this function
  //RooStudyManger forces a new TProof to be created and used,
  //This loses the link to the HS macros and I could not find an alternative solution
  //Th only difference is the follwing line
   void* p=(void*)gProof;
  // Check that PROOF initialization actually succeeeded
  if (p==0) {
    coutE(Generation) << "RooHSStudyManager::runProof(" << GetName() << ") ERROR initializing proof, aborting" << endl ;
    return ;
  }

  // Suppress GUI if so requested
  if (!showGui) {
    gROOT->ProcessLineFast(Form("((TProof*)0x%lx)->SetProgressDialog(0) ;",(ULong_t)p)) ;
  }

  // Propagate workspace to proof nodes
  coutP(Generation) << "RooHSStudyManager::runProof(" << GetName() << ") sending work package to PROOF servers" << endl ;
  gROOT->ProcessLineFast(Form("((TProof*)0x%lx)->AddInput((TObject*)0x%lx) ;",(ULong_t)p,(ULong_t)_pkg) ) ;

  // Run selector in parallel
  coutP(Generation) << "RooHSStudyManager::runProof(" << GetName() << ") starting PROOF processing of " << nExperiments << " experiments" << endl ;
			 
  gROOT->ProcessLineFast(Form("((TProof*)0x%lx)->Process(\"RooProofDriverSelector\",%d) ;",(ULong_t)p,nExperiments)) ;

  // Aggregate results data
  coutP(Generation) << "RooHSStudyManager::runProof(" << GetName() << ") aggregating results data" << endl ;
  TList* olist = (TList*) gROOT->ProcessLineFast(Form("((TProof*)0x%lx)->GetOutputList()",(ULong_t)p)) ;
  aggregateData(olist) ;

  // cleaning up                                                                                                                                           
  coutP(Generation) << "RooHSStudyManager::runProof(" << GetName() << ") cleaning up input list" << endl ;                                                   
  gROOT->ProcessLineFast(Form("((TProof*)0x%lx)->GetInputList()->Remove((TObject*)0x%lx) ;",(ULong_t)p,(ULong_t)_pkg) ) ;                                                   
  
}

