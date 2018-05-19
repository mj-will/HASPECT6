{
  ////////////////////////////////////////////////////////////////
  //Now want to sPlot using discriminatory variable M
  // THSsPlot* sP=new THSsPlot("splot",RF->GetWorkSpace());
  THSsPlot* sP=new THSsPlot("splot");
  //set output directory for plots, results, weights/...
  //sP->SetOutDir(RF->GetOutDir());
  sP->SetPlot(kFALSE);
  //Now load just the mass variable
  sP->LoadVariable("M[0,10]"); 
  //Make model
  //Signal first
  sP->Factory("Gaussian::sig( M, Mm[6,4,7], Mw[0.2,0.0001,3] )");
  //Load signal with nominal yield 1000
  sP->LoadSpeciesPDF("sig",1);
  //Flat background distributions
  sP->Factory("Uniform::bg( M )");
  sP->LoadSpeciesPDF("bg",1);
  
  //Load data for fitting
  sP->AddFitOption(PrintLevel(-1));
  //Fit data and model and calcualte sWeights
  sP->SetSaveWeights(kFALSE);
  sP->TotalPDF();
  //Add to list of splots, for factorised fits
  gsPlots->Add(sP);
  
  /////////////////////////////////////////////////////////
  //Now weight the phi distribution with the sweights and fit
  // THSRooFit* asym=new THSRooFit("phifit",RF->GetWorkSpace());
  THSRooFit* asym=new THSRooFit("phifit");
  //set output directory for plots, results, weights/...
  //asym->SetOutDir(RF->GetOutDir());
  // asym->SetPlot(kFALSE);
  //Load variables
  //Just Phi for fitting
  asym->LoadVariable("Phi[-3.14159,3.14159]");//should be same name as variable in tree  
  //asym->LoadVariable("Phi[-3.14159,0]");//should be same name as variable in tree  
  //Make model
 //Only signal now  
  asym->Factory("EXPR::asymmetry('1+A*cos(2*Phi)',Phi,A[0.,-1.5,1.5])"); 
  asym->LoadSpeciesPDF("asymmetry",1);
  asym->TotalPDF();
  HS::gRF=asym;
  HS::gRF->SetWeightName("sig"); //Set the weighted species

}
