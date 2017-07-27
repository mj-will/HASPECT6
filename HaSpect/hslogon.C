void HSfit();//Load hsfit classes
void HSdata();//Load hsdata classes
void HSselector(); //load hsselctor classes
void HSproject(TString pname); //load hsproject classes
void startproof(Int_t Nw); //intialise proof for hsselector classes
void LoadMacro(TString macro); //Load class via its source code
TString HSin(); //return inout void HSMacPath(TString opt)files directory
TString HSout(); //return inout files directory
TString HSproj(); //return project classname
void HSin(TString hsin){gSystem->Setenv("HSIN",hsin);} //set in files directory
void HSout(TString hsout){gSystem->Setenv("HSOUT",hsout);} //set out file
void HSproj(TString hsproj){gSystem->Setenv("HSPROJ",hsproj);} //set out file
void MakeAll();
void HSMacPath(TString opt);
void HSUserPath(TString opt);
#include <TProof.h> //MAke sure gProof can be seen here
TString MACPATH; //additional macro path needed for proof
Bool_t gISFARM=kFALSE;
TString THSPARTICLE("THSParticle.0.C");

void hslogon(){
  //Execute system .rootlogon.C script 
  if(gSystem->AccessPathName("~/.rootlogon.C")){};

  //Check if user source code directory defined
  TString HSANA=gSystem->Getenv("HSANA");
  TString HSUSER=gSystem->Getenv("HSUSER");
  //remove default link to THSParticle.0.h in case user defined THSParticle
  if(gSystem->Which(HSANA,"THSParticle.h"))
    gSystem->Exec(Form("rm $HSANA/THSParticle.h"));

  if(gSystem->Getenv("HSUSER")){
    //Option to include a THSParticle.C defintion in your own code
    //This should not be used with --particle which looks specifically
    //for a file in $HSANA
    gROOT->SetMacroPath(Form("%s:%s",HSUSER.Data(),gROOT->GetMacroPath()));
    gSystem->AddIncludePath(TString("-I")+HSUSER);
    //See if there is a user THSParticle class
    if(gSystem->Which(HSUSER,"THSParticle.C"))
      THSPARTICLE="THSParticle.C";
  }
  else{
    //Make a soft link to THSParticle.0.h, if another particle class is given this will be replaced
    gSystem->Exec(Form("ln -s $HSANA/THSParticle.0.h $HSANA/THSParticle.h"));
  }

  //get command line options first check if makeall
  for(Int_t i=1;i<gApplication->Argc();i++){
    TString opt=gApplication->Argv(i);
    //look for --proof=Nworkers optionif Nworkers not given all cores will be used
    if((opt.Contains("--makeall"))) MakeAll();
    
  }
  //check if additional macro dir given
   for(Int_t i=1;i<gApplication->Argc();i++){
    TString opt=gApplication->Argv(i);
    //look for --proof=Nworkers optionif Nworkers not given all cores will be used
    if((opt.Contains("--macrodir"))){
      HSMacPath(TString(opt(11,opt.Sizeof())));
    }
    if((opt.Contains("--particle"))){
      THSPARTICLE=(TString(opt(11,opt.Sizeof()))).Data();
      cout<<"THSPARTICLE= "<<THSPARTICLE<<endl;
      gSystem->Exec(Form("rm $HSANA/THSParticle.h"));
      TString THSPARTICLEH=THSPARTICLE;
      THSPARTICLEH.ReplaceAll(".C",".h");
      gSystem->Exec(Form("ln -s $HSANA/%s $HSANA/THSParticle.h",THSPARTICLEH.Data()));
   }
  }

  
   //get command line options first check if proof
  for(Int_t i=1;i<gApplication->Argc();i++){
    TString opt=gApplication->Argv(i);
    //look for --proof=Nworkers optionif Nworkers not given all cores will be used
    if((opt.Contains("--proof"))) startproof(TString(opt(8,opt.Sizeof())).Atoi());
    
  }
  
   //get command line options first check if rootbeer
  for(Int_t i=1;i<gApplication->Argc();i++){
    TString opt=gApplication->Argv(i);
    if((opt.Contains("--rootbeer"))) gROOT->ProcessLine(".L $ROOTBEER/RootBeerSetup.cxx");
  }

   //get command line options first check if farm job
  for(Int_t i=1;i<gApplication->Argc();i++){
    TString opt=gApplication->Argv(i);
    //look for --farm and copy all hsroot files to local hsana directory
    if((opt.Contains("--farm"))){
      gISFARM=kTRUE;
      gSystem->Exec(Form("mkdir hsana"));
      gSystem->Exec(Form("cp %s/*.h hsana/.",HSANA.Data()));
      gSystem->Exec(Form("cp %s/*.C hsana/.",HSANA.Data()));
      gSystem->Setenv("HSANA","./");
      if(gSystem->Getenv("RHIPO")){
	TString RHIPO=gSystem->Getenv("RHIPO");
	gSystem->Exec(Form("cp %s/THipo.h hsana/.",RHIPO.Data()));
	gSystem->Exec(Form("cp %s/THipo.C hsana/.",RHIPO.Data()));
	gSystem->Exec(Form("cp %s/Hipo2Root.C hsana/.",RHIPO.Data()));
	gSystem->Setenv("RHIPO","./");	
      }
      if(gSystem->Getenv("CHIPO")){
	TString CHIPO=gSystem->Getenv("CHIPO");
	gSystem->Exec(Form("cp %s/*.h hsana/.",CHIPO.Data()));
	gSystem->Exec(Form("cp %s/*.cpp hsana/.",CHIPO.Data()));
	gSystem->Setenv("CHIPO","./");	
      }
      gROOT->SetMacroPath(TString("hsana/:")+gROOT->GetMacroPath());
    }
  }
  
  //get command line options and call assiciated functions
  for(Int_t i=1;i<gApplication->Argc();i++){
    TString opt=gApplication->Argv(i);
    if(opt.Contains("--hsin")) {HSin(TString(opt(7,opt.Sizeof())));} //Set input tree directory
    if(opt.Contains("--hsout")) {HSout(TString(opt(8,opt.Sizeof())));} //Set input tree directory
    if((opt==TString("--hsfit"))) HSfit(); //Load fit classes
    if((opt==TString("--hsdata"))) HSdata(); //Load data classes
    if((opt==TString("--hssel"))) HSselector(); //Load selector classes
    if((opt.Contains("--hsproj"))) HSproject(TString(opt(9,opt.Sizeof()))); //Load project classes

    //don't load alter --particle option
    if(!(opt.Contains("--particle"))){
      if(opt.Contains("--")&&opt.Contains(".cxx")){opt.Remove(0,2); cout<<"Loading "<<opt<<endl;LoadMacro(opt);} //Load additional THS classes
      if(opt.Contains("--")&&opt.Contains(".C")){opt.Remove(0,2); cout<<"Loading "<<opt<<endl;LoadMacro(opt);} //Load additional THS classes
    }
  }	
  
}
void MakeAll(){

  TString HSANA=gSystem->Getenv("HSANA");
  TString CurDir=gSystem->Getenv("PWD");
  gSystem->Exec(Form("cd %s",HSANA.Data()));
  gSystem->Exec("rm *.so");

  // gROOT->LoadMacro("THSBins.C++");
  // gROOT->LoadMacro("THSWeights.C++");
  // gROOT->LoadMacro("THSRooFit.C++");
  // gROOT->LoadMacro("THSsPlot.C++");
  // //  gROOT->LoadMacro("THSParticle.C++");
  // gROOT->LoadMacro(THSPARTICLE+"++");
  // gROOT->LoadMacro("THSHisto.C++");
  // gROOT->LoadMacro("THSOutput.C++");
  // gROOT->LoadMacro("THSPartOutput.C++");
  // gROOT->LoadMacro("THSSkeleton.C++");
  // gROOT->LoadMacro("THSKinematics.C++");
  // gROOT->LoadMacro("THSProject.C++");
  // gROOT->LoadMacro("THSDataManager.C++");
  // gROOT->LoadMacro("THSLundReader.C++");

  gSystem->Exec(Form("cd %s",CurDir.Data()));
}
void HSfit(){
  cout<<"Loading HSFit classes"<<endl;
  //set prompt
  if(!gProof)((TRint*)gROOT->GetApplication())->SetPrompt("hsroot [%d] ");
  //Load extra classes for roofit
  LoadMacro("THSBins.C"); 
  LoadMacro("THSWeights.C");
  LoadMacro("RooHSEventsPDF.C");
  LoadMacro("RooHSEventsHistPDF.C");
  LoadMacro("THSRooFit.C");
  LoadMacro("THSsPlot.C");
  LoadMacro("RooHS1StepStudy.C");
   //  LoadMacro("RooHSStudyManager.C");
}
void HSselector(){
  cout<<"Loading HSSel classes"<<endl;
 //set prompt
  if(!gProof)((TRint*)gROOT->GetApplication())->SetPrompt("hsroot [%d] ");
   //Load classess for selector analysis
  //particle container class
  LoadMacro("THSBins.C");
  LoadMacro("THSWeights.C");
  // LoadMacro("THSParticle.C");
  //if(!TClass::GetClass("THSParticle"))LoadMacro(THSPARTICLE);
  LoadMacro(THSPARTICLE);
  LoadMacro("THSHisto.C");
  LoadMacro("THSOutput.C");
  // LoadMacro("THSPartOutput.C");
  LoadMacro("THSSkeleton.C");
  LoadMacro("THSKinematics.C");

  //qfactor class
  //gROOT->LoadMacro("THSEventWeighter.C+");
  //lps class
  //gROOT->LoadMacro("THSLongPS.C+");
}

void HSproject(TString pname){
  HSproj(pname);
  if(!TClass::GetClass("THSParticle")) LoadMacro("THSParticle.C");
  LoadMacro(THSPARTICLE);
  LoadMacro("THSWeights.C");
  LoadMacro("THSDataManager.C");
  LoadMacro("THSKinematics.C");
  LoadMacro("THSProject.C");
  LoadMacro(pname+".C");

}
void HSdata(){
  if(gSystem->Getenv("RHIPO"))
    gROOT->ProcessLine(TString(".x ")+gSystem->Getenv("RHIPO")+"/Hipo2Root.C");
  cout<<"THSPARTICLE= "<<THSPARTICLE<<endl;
 
  LoadMacro("THSWeights.C");
  //if(!TClass::GetClass("THSParticle"))   LoadMacro("THSParticle.C");
  LoadMacro(THSPARTICLE);
  LoadMacro("THSDataManager.C");
  LoadMacro("THSLundReader.C");
  LoadMacro("THSRADSReader.C");
  if(gSystem->Getenv("RHIPO"))
    LoadMacro("THSHipoReader.C");

}
void startproof(Int_t Nw){
  cout<<"Loading Proof setup"<<endl;
  //set prompt
  ((TRint*)gROOT->GetApplication())->SetPrompt("hsproof [%d] ");
  TProof::Open("://lite");
  // gProof->AddEnvVar("PROOF_INITCMD","echo setenv HSOUT test");
  // cout<<"HSOUT IS "<<gSystem->Getenv("HSOUT")<<endl;
  // plite->AddIncludePath(gInterpreter->GetIncludePath());
  //  tree->SetProof();
  //  plite->SetParameter("PROOF_UseTreeCache", 0); //turn off caching =0
  // plite->SetParameter("PROOF_CacheSize", 1000M);
  // gProof->SetParameter("PROOF_UseMergers", 0);//seems to be a bit faster initialising, for when you have lots of histograms
  if(Nw) gProof->SetParallel(Nw); //set number of workers if specifed on command line
  //plite->SetParallel(UsePROOF); //restrict workers
}
void LoadMacro(TString macro){
  //Different methods depending on whether proof or not
  TString HSANA=gSystem->Getenv("HSANA");
  TString HSUSER=gSystem->Getenv("HSUSER");

  cout<<"Loading hsmacro "<<macro<<endl;
  if(gProof) {
    if(gSystem->Which("./",macro)){
      macro=macro;
    }	
    else if(gSystem->Which(HSUSER,macro)){
      macro=HSUSER+"/"+macro;
    }
    else  if(gSystem->Which(MACPATH,macro)){
      macro=MACPATH+"/"+macro;
    }
    else  if(gSystem->Which(HSANA,macro)){
      macro=HSANA+"/"+macro;
    }
    else {cout<<"Can't find macro "<<macro<<endl; return;}
    gProof->Load(macro+"+");
  }
  else gROOT->LoadMacro(macro+"+");//don't use HSANA in case wnat to overwrite with macro in current directory
  
}
TString HSproj(){
  if(!gSystem->Getenv("HSPROJ")) cout<<"Warning no HSPROJ env variable defined but hsproj() called..."<<endl;
  return TString(gSystem->Getenv("HSPROJ"));
}
TString HSin(){
  if(!gSystem->Getenv("HSIN")) cout<<"Warning no HSIN env variable defined but hsin() called..."<<endl;
  return TString(gSystem->Getenv("HSIN"))+"";
}
TString HSout(){
  if(!gSystem->Getenv("HSOUT")) cout<<"Warning no HSOUT env variable defined but hsout() called..."<<endl;
  if(gProof) cout<<"Warning : Using proof so need full path to output"<<endl;
  return gSystem->Getenv("HSOUT");
}
void HSMacPath(TString opt){
  cout<<"Adding Macro Path "<<opt<<endl;
  MACPATH=opt;
  gROOT->SetMacroPath(Form("%s:%s",gROOT->GetMacroPath(),MACPATH.Data()));
  gSystem->AddIncludePath(TString("-I")+MACPATH);
}
