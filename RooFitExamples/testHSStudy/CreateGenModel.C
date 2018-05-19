{
  
  THSRooFit* RF=new THSRooFit("input");
  //set output directory for plots, results, weights/...
  RF->SetOutDir("/.");

  //Load variables
  //Two dimension Mass M and angle Phi
  RF->LoadVariable("M[0,10]");//should be same name as variable in tree  
  RF->LoadVariable("Phi[-3.14159,3.14159]");//should be same name as variable in tree  
 
  //Make model
  //Signal first
 RF->Factory("Gaussian::MSignal( M, SIMm[6,4,7], SIMw[0.2,0.0001,3] )");
 RF->Factory("EXPR::PSignal('1+A*cos(2*Phi)',Phi,A[0.92,-1.5,1.5])"); 
 RF->Factory("PROD::Signal(MSignal|Phi,PSignal)");
 //RF->Factory("PROD::Signal(PSignal|M,MSignal)");
 //RF->Factory("PROD::Signal(PSignal,MSignal)");

 //Load and draw signal with nominal yield 1000
 RF->LoadSpeciesPDF("Signal",8000);
 RF->PlotModel("M","Signal");
 RF->PlotModel("Phi","Signal");


 //Flat background distributions
 // RF->Factory("Uniform::MBG( M )");
 // RF->Factory("Uniform::PBG(Phi)"); 
 // RF->Factory("PROD::BG(MBG,PBG)");
 //Flat background distributions
 RF->Factory("Uniform::MBG( M )");
 RF->Factory("EXPR::PBG('4+Phi',Phi)"); 
 RF->Factory("PROD::BG(MBG,PBG)");


 //Load and draw background with nominal yield 5000
 RF->LoadSpeciesPDF("BG",20000);
 RF->PlotModel("M","BG");
 RF->PlotModel("Phi","BG");

 //combine Signal and background for over model of data
 //This is an extended PDF with yield parameters (set to 1000, and 5000 initialy)
 RF->DefineSets();
 RF->TotalPDF();
 //Plot combined model
 RF->PlotModel("M","inputTotalPDF0");
 RF->PlotModel("Phi","inputTotalPDF0");

 HS::gRF=RF;
}
