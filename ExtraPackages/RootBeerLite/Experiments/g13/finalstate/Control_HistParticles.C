{
  //optional Set input file directory, could be done with --hsin or setenv HSIN
  //HSin("/w/work1/jlab/hallb/g13/K0L18/runs_3.3_1.3/");
  HSin("/w/work1/jlab/hallb/g13/K0L18/K0LSim_11_23/");
  //optional set ouput directory or file.
  //Directory=>1 out file for every in file
  //file=> 1 total output file
  HSout("/scratch/dglazier/K0L18/K0LSim.root");
  //HSout("/scratch/dglazier/K0L18/runs_3.3_1.3.root");
  
  //Set up the input chain to be analysed,could remove HSin() if you want
  TChain* tree=new TChain("HSParticles","datachain");
  //tree->Add(HSin()+"Run_054145.root");
  tree->Add(HSin()+"DSTJob14659644.farmpbs14.root");

  //if using proof
  //Give proof destination file (env variables can't be read directly
  if(gProof) {gProof->AddInput(new TNamed("HSOUT",HSout().Data()));gProof->AddInput(new TNamed("HSPROOF","Using Proof"));tree->SetProof();}

  gBenchmark->Start("SelectorTimer");

  tree->Process("HistParticles.C+");
  
  gBenchmark->Stop("SelectorTimer");
  gBenchmark->Print("SelectorTimer");
  tree->PrintCacheStats();

  //Not actually sure if this is needed or not...
  if(gProof)gProof->ClearData(TProof::kUnregistered | TProof::kForceClear);
}
