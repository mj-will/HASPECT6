#ifndef THSMVAPREP_h
#define THSMVAPREP_h

#include <TTree.h>
#include "THSMVA.h"


class THSMVAPrep : public THSMVA {

 public :
  THSMVAPrep();
  virtual ~THSMVAPrep();
  
 protected :

  TTree* fTrainTree=nullptr;
  
 public:

  void SetTree(TTree* tree){fTrainTree=tree;};
};

#endif //
