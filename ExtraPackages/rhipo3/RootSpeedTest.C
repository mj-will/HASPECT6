#include <TTree.h>
#include <TFile.h>
#include <TBranch.h>
#include <TBenchmark.h>

void RootSpeedTest(TString filename){
  TFile* file=new TFile(filename);
  TTree* tree0=(TTree*)file->Get("hipo2root");

  //tree0->SetBranchStatus("*",0);
  //tree0->SetBranchStatus("REC_Event*",1);
  //tree0->SetBranchStatus("REC_Particle*",1);

  TBranch* branch_px=tree0->GetBranch("REC_Particle_px");
  vector<float> *vpx=0;
  branch_px->SetAddress(&vpx);
  TBranch* branch_py=tree0->GetBranch("REC_Particle_py");
  TBranch* branch_pz=tree0->GetBranch("REC_Particle_pz");
  //TBranch* branch_=tree0->GetBranch("REC_Particle_");
  TBranch* branch_pid=tree0->GetBranch("REC_Particle_pid");
  TBranch* branch_charge=tree0->GetBranch("REC_Particle_charge");
  TBranch* branch_status=tree0->GetBranch("REC_Particle_status");
  TBranch* branch_vx=tree0->GetBranch("REC_Particle_vx");
  TBranch* branch_vy=tree0->GetBranch("REC_Particle_vy");
  TBranch* branch_vz=tree0->GetBranch("REC_Particle_vz");
  TBranch* branch_beta=tree0->GetBranch("REC_Particle_beta");
  // TBranch* branch_=tree0->GetBranch("REC_Particle_");
  TBranch* branch_chi2pid=tree0->GetBranch("REC_Particle_chi2pid");

  
  gBenchmark->Start("tree all");
   for(Int_t i=0;i<tree0->GetEntries();i++)
    tree0->GetEntry(i);
  gBenchmark->Stop("tree all");
  gBenchmark->Print("tree all");

  gBenchmark->Start("tree");
  tree0->SetBranchStatus("*",0);
  //  tree0->SetBranchStatus("REC_Event*",1);
  tree0->SetBranchStatus("REC_Particle*",1); 
  for(Int_t i=0;i<tree0->GetEntries();i++)
    tree0->GetEntry(i);
  gBenchmark->Stop("tree");
  gBenchmark->Print("tree");

  gBenchmark->Start("branch");
  for(Int_t i=0;i<branch_px->GetEntries();i++){
    branch_px->GetEntry(i);
    branch_py->GetEntry(i);
    branch_pz->GetEntry(i);
    branch_pid->GetEntry(i);
    branch_charge->GetEntry(i);
    branch_status->GetEntry(i);
    branch_vx->GetEntry(i);
    branch_vy->GetEntry(i);
    branch_vz->GetEntry(i);
    branch_beta->GetEntry(i);
    branch_chi2pid->GetEntry(i);
  }
  gBenchmark->Stop("branch");
  gBenchmark->Print("branch");


}
