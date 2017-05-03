//Make own study manager class as cannot load classes automatically
//in RooStudyManager proof sessions
//The class is a direct copy with some addtions to the runProof function
#ifndef HS_STUDY_MANAGER
#define HS_STUDY_MANAGER

#include "RooStudyManager.h"


class RooHSStudyManager : public RooStudyManager {
public:

  RooHSStudyManager(RooWorkspace& w) ;
  RooHSStudyManager(RooWorkspace& w, RooAbsStudy& study) ;
  RooHSStudyManager(const char* studyPackFileName) ;

  // PROOF-based paralllel running
  void runMyProof(Int_t nExperiments, const char* proofHost="", Bool_t showGui=kTRUE) ;
 
protected:

	
  ClassDef(RooHSStudyManager,1) // A general purpose workspace oriented parallelizing study manager
} ;


#endif

