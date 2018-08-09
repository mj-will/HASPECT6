/**
 * \class THS2pi
 * 
 * Template final class .
 * 
 * Users should create their own analysis specific project classes.
 * 
 */



#include "TDatabasePDG.h"
#include "THS2pi.h"
#include <algorithm>


THS2pi::THS2pi(){
  SetVerbose(1);
  // CheckCombi(); //comment out to remove messages
  
  //Set final state detected particles
  //AddParticle(particle,true/false you want to write in final vector, genID for linking to generated truth value)
  //Note if particle is added to final with a valid genID it will be used
  //to determine the correct permutation of the simulated event
  AddParticle(&fElectron,kTRUE,3);
  AddParticle(&fProton,kTRUE,2);
  AddParticle(&fPip,kTRUE,0);
  AddParticle(&fPim,kTRUE,1);

  PrepAddParticle(&fElectron);
  PrepAddParticle(&fProton);
  PrepAddParticle(&fPip);
  PrepAddParticle(&fPim);
    
  // for non 2pi
  //fMVAPrep.SetParticles({"El", "P"});
  //fMVAPrep.SetVariables({"E", "P"});
  //fMVAPrep.SetTypes({"F", "F"})

  //Set final state parents
  
  TString PID("NONE"); //set this to which particles you want to id via pdg code alone, or set it in individual AddTopology
  TString INCLUSIVE("ALL");//set this to which particles you want reaction to be inclusive of, or set it in individual AddTopology "ALL"=> completely inclusive

  //include topology for analysis and get index
  AddTopology("e-:pi+:pi-:proton",
             bind(&THS2pi::Init_Iter0, this),
             bind(&THS2pi::Topo_0, this),
             PID,INCLUSIVE);

  AddTopology("e-:pi+:proton",
             bind(&THS2pi::Init_Iter1, this),
             bind(&THS2pi::Topo_1, this),
             PID,INCLUSIVE);

  AddTopology("e-:pi-:proton",
             bind(&THS2pi::Init_Iter2, this),
             bind(&THS2pi::Topo_2, this),
             PID,INCLUSIVE);

  AddTopology("e-:pi+:pi-",
             bind(&THS2pi::Init_Iter3, this),
             bind(&THS2pi::Topo_3, this),
             PID,INCLUSIVE);

  THSFinalState::InitFS();
}

void THS2pi::FileStart(){
  //May be called when a new file is opened
  fTrigger.SetParticles(frDetParts); //the detected particles
  fTrigger.SetEventInfo(fEventInfo);//once per event info
  fTrigger.SetRunInfo(fRunInfo);//once per run info

  //Configure the cuts (default are delta timing cuts in ns)
  fCuts.SetElCut(2,2); //Just FT,FD
  fCuts.SetPionPCut(0,2,0,2);//FT,FDTOF,CD,FDCAL
  fCuts.SetPionMCut(0,2,0,2);//FT,FDTOF,CD,FDCAL
  fCuts.SetProtCut(0,2,1,2);//FT,FDTOF,CD,FDCAL
  fCuts.SetGammaCut(2,2,0,2);//FT,FDTOF,CD,FDCAL

  //Initilaise THSMVA stuff
  // add config stuff here
  if (fIsTrain){

      std::cout<<"Setting MVA trees..."<<std::endl;
      fMVAPrep.SetNames();
      // add splits: name, exact variable names, pointers, values
      //fMVATrain.AddSplit("Topo0", "Topo", &fTopoID, 0);
      //fMVATrain.AddSplit("Topo1", "Topo", &fTopoID, 1);
      //fMVATrain.AddSplit("Topo2", "Topo", &fTopoID, 2);
      //fMVATrain.AddSplit("Topo3", "Topo", &fTopoID, 3);
      //fMVATrain.PrintSplits();
      //fSplits = fMVATrain.GetSplits(); 
      //
      Method method0;
      method0.fMethodName = "BDT0";
      method0.fMethodType = TMVA::Types::kBDT;
      method0.fMethodParameters = "!H:!V:NTrees=1200:MinNodeSize=2.5%:MaxDepth=2:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20";
      fMVATrain.AddMethod(method0);
      //
      Method method1;
      method1.fMethodName = "BDT1";
      method1.fMethodType = TMVA::Types::kBDT;
      method1.fMethodParameters = "!H:!V:NTrees=1200:MinNodeSize=2.5%:MaxDepth=4:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20";
      fMVATrain.AddMethod(method1);
      Method method2;
      method2.fMethodName = "MLP";
      method2.fMethodType = TMVA::Types::kMLP;
      method2.fMethodParameters = "H:!V:NeuronType=tanh:VarTransform=N:NCycles=600:HiddenLayers=N+5:TestRate=5:!UseRegulator";
          //"H:!V:NeuronType=ReLU:EstimatorType=CE:learningRate=0.01:VarTransform=I:NCycles=20:HiddenLayers=64:TestRate=5:BatchSize=128:BPMode=batch";
      
      //fMVATrain.AddMethod(method2);

      Method method3;
      method3.fMethodName = "DNN";
      method3.fMethodType = TMVA::Types::kDNN;
      method3.fMethodParameters = "!H:V:ErrorStrategy=CROSSENTROPY:VarTransform=N:WeightInitialization=XAVIERUNIFORM:Layout=RELU|64,LINEAR:TrainingStrategy=LearningRate=1e-2,Momentum=0.9,Repetitions=1,ConvergenceSteps=300,BatchSize=128,TestRepetitions=15,WeightDecay=0.0,Regularization=NONE,DropConfig=0.0+0.1,DropRepetitions=1,Multithreading=True";
      fMVATrain.AddMethod(method3);

      std::cout<<"  Finished setting methods and splits"<<std::endl;
      fMVAPrep.SetTree(fFinalTree);
      fMVATrain.SetTrainTree(fFinalTree);
  }
  else{
  }
  
  
  if(THSFinalState::frGenParts) fTrigger.SetSim();//Should get this from RunInfo but not correct in EB at the moment
  
}

//Define topology Iterator functions
void THS2pi::Init_Iter0(){
  //THSParticle iterator initialisation
  //For topology e-:pi+:pi-:proton

   AutoIter(); //Let finalstate try and work out the iterattor for you, you can remove this if you want to do it yourself
}
void THS2pi::Init_Iter1(){
  //THSParticle iterator initialisation
  //For topology e-:pi+:proton

   AutoIter(); //Let finalstate try and work out the iterattor for you, you can remove this if you want to do it yourself
}
void THS2pi::Init_Iter2(){
  //THSParticle iterator initialisation
  //For topology e-:pi-:proton

   AutoIter(); //Let finalstate try and work out the iterattor for you, you can remove this if you want to do it yourself
}
void THS2pi::Init_Iter3(){
  //THSParticle iterator initialisation
  //For topology e-:pi+:pi-

   AutoIter(); //Let finalstate try and work out the iterattor for you, you can remove this if you want to do it yourself
}
// void THS2pi::Init_IterX(){
//  THSParticleIter* diter=CreateParticleIter(fTID_X,&fVecY,NPARTICLES);
//  ...
// }
//Define topology functions
void THS2pi::Topo_0(){
  //For topology e-:pi+:pi-:proton
  //if(fElectron.Detector()>0) {fGoodEvent=kFALSE;return;} //Put some cuts on particle detectors
  //Define starttime from electron candidate
  if(fElectron.Detector()<-1)fElectron.SetPath(1.97);
  // fPip.SetPath(fPip.Path()*0.96);
  // fPim.SetPath(fPim.Path()*0.98);
  // fProton.SetPath(fProton.Path()*0.95);
  Float_t stime=fTrigger.StartTime(&fElectron);
  fTrigger.SubtractStartTime(&fElectron,&fProton,&fPip,&fPim);

  HSLorentzVector miss=fBeam+fTarget-fElectron.P4()-fProton.P4()-fPip.P4()-fPim.P4();
  fMissMass2=miss.M2();
  fMissMass=miss.M();

 
}
void THS2pi::Topo_1(){
  //For topology e-:pi+:proton
  //if(fElectron.Detector()>0) {fGoodEvent=kFALSE;return;} //Put some cuts on particle detectors
  if(fElectron.Detector()<-1)fElectron.SetPath(1.97);
  Float_t stime=fTrigger.StartTime(&fElectron);
  fTrigger.SubtractStartTime(&fElectron,&fProton,&fPip);
  //Reconstruct missing or combined particles
  HSLorentzVector miss=fBeam+fTarget-fElectron.P4()-fProton.P4()-fPip.P4();
  fMissMass2=miss.M2();
  fMissMass=miss.M();
  fPim.SetP4(miss);
  fPim.TakePDGMass();
 }
void THS2pi::Topo_2(){
  //For topology e-:pi-:proton
  //if(fElectron.Detector()>0) {fGoodEvent=kFALSE;return;} //Put some cuts on particle detectors
  if(fElectron.Detector()<-1)fElectron.SetPath(1.97);
  Float_t stime=fTrigger.StartTime(&fElectron);
  fTrigger.SubtractStartTime(&fElectron,&fProton,&fPim);

  HSLorentzVector miss=fBeam+fTarget-fElectron.P4()-fProton.P4()-fPim.P4();
  fMissMass2=miss.M2();
  fMissMass=miss.M();
  fPip.SetP4(miss);
  fPip.TakePDGMass();
}
void THS2pi::Topo_3(){
  //For topology e-:pi+:pi-
  //if(fElectron.Detector()>0) {fGoodEvent=kFALSE;return;} //Put some cuts on particle detectors
   if(fElectron.Detector()<-1)fElectron.SetPath(1.97);
   Float_t stime=fTrigger.StartTime(&fElectron);
  fTrigger.SubtractStartTime(&fElectron,&fPip,&fPim);
  HSLorentzVector miss=fBeam+fTarget-fElectron.P4()-fPip.P4()-fPim.P4();
  fMissMass2=miss.M2();
  fMissMass=miss.M();
  fProton.SetP4(miss);
  fProton.TakePDGMass();

 
}
// void THS2pi::Topo_X(){
// }

void THS2pi::Kinematics(){
  //configure trigger for this event
  fTrigger.ReadParticles();
  //Do calculations if Good Event
  //if(fIsTMVA) TMVAFill();
  if (fIsTMVA){
      PrepFillVars(); // for trains
      fMVAPrep.RemoveNaNs();
      // TODO : figure out how to get correct flag
      //std::cout<<fGotCorrectOne<<std::endl;
      //if (fGotCorrectOne==1){fSignalCount++;};
      //if (fCorrect==0){fBackgroundCount++;};

      //fMVAApp.GetResponse();

  }
  else{
  
      AppFillVars();
      fMVAApp.ProcessEvent();
  }

}

Int_t THS2pi::CheckSignalCount(TTree* tree){
    //tree->Print();
    if (!fSplits.empty()){
        fSplitCount = 0;
        for (UInt_t i=0; i<fSplits.size(); i++){
            tree->Draw(">>eventlist","Correct==1&"+fSplits[i].GetTreeSplit(),"goff"); 
            fEventList = (TEventList*)gDirectory->Get("eventlist");
            if (fEventList){
                fSignalCount = fEventList->GetN();
            }
            else fSignalCount = 0;
            std::cout<<"    Signal count for split "<<i<<": "<<fSignalCount;
            // check if criteria met for this split
            if (fSignalCount >= fTotalEvents){ fSplitCount++;};
        }
        std::cout<<" "<<std::endl;
        // only return true if all splits have met criteria
        if (fSplitCount == fSplits.size()) return kTRUE;
        else return kFALSE;
    }
    else{
        tree->Draw(">>eventlist","Correct==1","goff"); 
        fEventList = (TEventList*)gDirectory->Get("eventlist");
        //std::cout<<"Getting number od entries"<<std::endl;
        if (fEventList){
            fSignalCount = fEventList->GetN();
        }
        else fSignalCount = 0;
        std::cout<<"    Signal count: "<<fSignalCount<<std::endl;
        if (fSignalCount >= fTotalEvents)return kTRUE;
        else return kFALSE;
    }
}

//////////////////////////////////////////////////
/// Define conditions for track to be considered
/// good in event. Adding conditions on junk tracks
///  can greatly reduce combitorials etc.
/// kFALSE=> track ignored completely
Bool_t THS2pi::CheckParticle(THSParticle* part){
  if(part->Detector()<0)return kTRUE; //All FT tracks
  if(part->PDG()==45)return kFALSE; //? what are these?
  if(part->Time()==0)return kFALSE;   //Track needs time
  if(part->Charge()&&part->DeltaE()<2)return kFALSE; //Charged track needs deltaE>2

  return kTRUE;
}

void THS2pi::FinalStateOutTree(TTree* tree){
  fIsTMVA=kFALSE;
  THSFinalState::fFinalTree=tree;
  fMVAApp.SetOutputTree(tree);
  //tree->Branch("Final",&fFinal);//If you want to save the final THSParticles
  tree->Branch("MissMass2",&fMissMass2,"MissMass2/D");
  tree->Branch("MissMass",&fMissMass,"MissMass/D");
  tree->Branch("Topo",&fTopoID,"Topo/I");
  tree->Branch("NPerm",&fNPerm,"NPerm/I");
  tree->Branch("NDet",&fNDet,"NDet/I");
  tree->Branch("Correct",&fCorrect,"Correct/I");

}
void THS2pi::TMVAOutTree(TTree* tree){
  THSFinalState::fFinalTree=tree;
  fIsTMVA=kTRUE;
  tree->Branch("MissMass2",&fMissMass2,"MissMass2/D");
  tree->Branch("MissMass",&fMissMass,"MissMass/D");
  tree->Branch("Topo",&fTopoID,"Topo/I");
  tree->Branch("NPerm",&fNPerm,"NPerm/I");
  tree->Branch("NDet",&fNDet,"NDet/I");
  //tree->Branch("Detector",&fDetector,"Detector/I");
  tree->Branch("Correct",&fCorrect,"Correct/I");

  fMVAPrep.SetTree(tree);
  fMVAPrep.SetBranches();

  fMVATrain.SetTrainTree(tree);
  fMVAApp.SetAppTree(tree);

}

void THS2pi::SetApplication(THSMVA* setup){

    fMVAApp.Init(setup);
    fMVAApp.UpdateSplit("Topo0", &fTopoID);
    fMVAApp.UpdateSplit("Topo1", &fTopoID);
    fMVAApp.UpdateSplit("Topo2", &fTopoID);
    fMVAApp.UpdateSplit("Topo3", &fTopoID);
    fMVAApp.PrintSplits();
    fMVAApp.AddReaders();
    fMVAApp.SetReaders();
    fMVAApp.SetOutput();
}

/**
 * Add particles to vector
 *
 */

void THS2pi::PrepAddParticle(THSParticle *part){
    fParticles.push_back(part);
}


/**
 * Fill variable vector
 *
 */

void THS2pi::PrepFillVars(){

    fNParts = fParticles.size();

    for (UInt_t i = 0; i<fNParts; i++){
        fMVAPrep.AddVarsFromParticle(fParticles[i], i);
    }
}

/**
 * Fill the variables for application
 *
 */

void THS2pi::AppFillVars(){

    fNParts = fParticles.size();

    for (UInt_t i = 0; i<fNParts; i++){
        fMVAApp.AddVarsFromParticle(fParticles[i], i);
    }
}

/**
 * Set the number of training and testing events
 *
 */

void THS2pi::SetNEvents(Int_t N){
    // set number of events
    fMVATrain.SetNTrain(N);
    fMVATrain.SetNTest(N);
    // set total signal events for counter
    fTotalEvents = 2*N;
}

void THS2pi::SetNEvents(Int_t NTrain, Int_t NTest){
    // set number of events
    fMVATrain.SetNTrain(NTrain);
    fMVATrain.SetNTest(NTest);
    fTotalEvents = NTrain + NTest;
}
