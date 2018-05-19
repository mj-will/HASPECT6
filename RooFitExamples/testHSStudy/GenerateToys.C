// setenv FILEDIR $PWD/Toys_A
//root --hsfit  GenerateToys.C 
{

  gROOT->LoadMacro("CreateGenModel.C");

  RF->SetOutDir(gSystem->Getenv("FILEDIR"));
  //Now generate toys...
  RF->GenerateToys(100);

}
