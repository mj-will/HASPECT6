//--Author      DI Glazier 30/06/2014
//--Rev
//--Update
//--Description
//HASPECT Event Reconstruction
//THSSkeleton
//Produces skeleton selector code for particular tasks

#ifndef THSSkeleton_h
#define THSSkeleton_h

#include <TObjArray.h>
#include <TObject.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TObjString.h>
#include <TMacro.h>

class THSSkeleton :public TObject  {

 private :
  Bool_t fIsHisto=kFALSE;  //use THSHisto?
  Bool_t fIsAppendTree=kFALSE; //Append branches to the input tree
  Bool_t fIsNewTree=kFALSE;  //Output a brand new tree
  Bool_t fIsHSTree=kFALSE;   //Use THSOuput to THSParticle interface (probably not)
  Bool_t fIsQval=kFALSE;   //Use Qvale event weighting algorithm
  Bool_t fIsWeights=kFALSE;   //Use THSWeights event weighting 
  Bool_t fIsFinalState=kFALSE;   //Use THSProject 
  Bool_t fIsFinalStatePerm=kFALSE;   //Use THSProject Permutations
  Bool_t fMadeSelector=kFALSE;
  Int_t fNLPS=0;   //Use Longitidinal phase space class with NLPS particles

  TString fFileName;   // The input filename containing the tree
  TString fTreeName;   // The name of the tree
  TString fOutName;   // The name of the output directory or file
  TString fSelName;    // The name of the selector to be produced
  TFile* fFile; //The tree file object
  TTree* fTree; // the tree object
  TMacro fCurMacro;
  Int_t fPlace;
  TString fOption;
  TString fFinalName;
  TString fFinalTopo;
  TString fFinalParts;
  TString fFinalParents;
  
 public :
  
 THSSkeleton()  : fFile(0),fTree(0),fPlace(0){ }
 virtual ~THSSkeleton() {}

 public :
  void SetHisto(Bool_t is=kTRUE){fIsHisto=is;}
  void SetAppendTree(Bool_t is=kTRUE){fIsAppendTree=is;}
  void SetNewTree(Bool_t is=kTRUE){fIsNewTree=is;}
  void SetWeights(Bool_t is=kTRUE){fIsWeights=is;}
  void SetLPS(Int_t is=0){fNLPS=is;}
  void SetFinalState(TString proj,Bool_t perm=kFALSE){fFinalName=proj;fIsFinalState=kTRUE;fIsFinalStatePerm=perm;}
  void SetFinalStateTopo(TString topos){fFinalTopo=topos;};
  void SetFinalStateParts(TString finals){fFinalParts=finals;};
  void SetFinalStateParents(TString finals){fFinalParents=finals;};
  void SetFileName(TString name){fFileName=name;}
  void SetSelName(TString name){fSelName=name;}
  void CreateSelector(TString selname,TString filename,TString treename,TString opt="");
  void AddHSOutput();
  void HSOut_h();
  void HSOut_C();
  void HSOut_Control();
  void HSHisto();
  void HSAppend();
  void HSNewTree();
  void HSWeights();
  void HSLPS();
  void HSFinalState();
  void CreateMyFinalState();
  
  //RooFit classes
  void CreateRooFitEventsPDF(TString pdfName,TString obsNames,TString parNames);
  
  void AddLineAfter(TString line0,TString line1,Int_t off=0);
  void ContinueLineAfter(TString line1,Int_t off=0);
  void MoveToLine(TString line0);
  TString FindNextLineLike(TString linelike);
  void ReplaceMacroText(TString text0,TString text1);
  void ReplaceAllMacroText(TString text0,TString text1);
  void ReplaceInCurrLine(TString text0,TString text1);
};


#endif 
