// setenv FILEDIR $PWD/Toys_A
//root --hsfit --RooHSSphHarMoments.cxx GenerateToys.C 
{

  gROOT->LoadMacro("CreateModel.C");

  RF->SetOutDir(gSystem->Getenv("FILEDIR"));
  //Justr draw the PDF with given moments
  TCanvas* can1=new TCanvas();
  ((TH2F*)RF->GetModel()->createHistogram("Z,Phi",50,50))->Draw("col1");
  //Give to RF for saving
  RF->GetPlots()->Add(can1);
  
  RF->SavePlots("");
  //Now generate toys...
  RF->GenerateToys(10);

}
