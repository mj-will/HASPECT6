//For perparing jsub files for jlab farm
//Example usage :
//root -l $RHIPO/MakeFarmJob.C --indir=/volatile/clas12/data/rg-a/.../ --run=2835 --jsub=SubmitToFarmTEMPLATE.jsub --nfiles=2 --outdir=/volatile/clas12/.../ --tperfile=300
{
  TString INDIR;
  TString OUTDIR;
  TString RUNNO;
  TString FARMFILE;
  Int_t NFILES=0;
  Int_t TperFILE=300; // time to analysis a file
  //Get command line arguments to configure farm job
  Int_t argc=gApplication->Argc();
  char** argv=gApplication->Argv();
  for(Int_t i=1;i<argc;i++){
    TString cmd=argv[i];
    if(cmd.Contains("--indir=")){
      INDIR=cmd(8,cmd.Sizeof());
      if(INDIR[INDIR.Sizeof()-2]!='/')
	INDIR.Append("/");
      cout<<"Taking files from "<<INDIR<<endl;
    }
    if(cmd.Contains("--outdir=")){
      OUTDIR=cmd(9,cmd.Sizeof());
    }
    if(cmd.Contains("--run=")){
      RUNNO=cmd(6,cmd.Sizeof());
      if(RUNNO==TString("GEMC"))
	cout<<"Analysing gemc runs "<<endl;
      else{     
	while(RUNNO.Sizeof()<7)
	  RUNNO.Prepend("0");
	cout<<"Analysing run number "<<RUNNO<<endl;
      }
    }
    if(cmd.Contains("--jsub=")){
      FARMFILE=cmd(7,cmd.Sizeof());
      TString rhipo=gSystem->Getenv("RHIPO");
      FARMFILE.Prepend(rhipo+"/");
      cout<<"Using template script "<<FARMFILE<<endl;
    }
    if(cmd.Contains("--nfiles=")){
     TString sNFILES=cmd(9,cmd.Sizeof());
     NFILES=sNFILES.Atoi();
     cout<<"Going to group "<<NFILES<<" files per job"<<endl;
    }
    if(cmd.Contains("--tperfile=")){
     TString sTIME=cmd(11,cmd.Sizeof());
     TperFILE=sTIME.Atoi();
     cout<<"Assume will take "<<TperFILE<<" s per job"<<endl;
    }
   }
  if(INDIR.Sizeof()==1) {cout<<"No input directory, use --indir=/some/directory/with/hipo/files"<<endl;exit(0);}
  if(OUTDIR.Sizeof()==1) {cout<<"No ouput directory, use --outdir=/some/directory/to/put/root/files"<<endl;exit(0);}
  if(RUNNO.Sizeof()==1) {cout<<"No run number, use --run=XXXXXX"<<endl;exit(0);}
  if(FARMFILE.Sizeof()==1) {cout<<"No template jsub file, use --jsub=SomeSubmitFile.jsub"<<endl;exit(0);}

  //Add run name to output directory
  OUTDIR+="/Run";
  OUTDIR+=RUNNO;
  gSystem->Exec(Form("mkdir -p %s",OUTDIR.Data()));
  //Get List of data files to submit
  void *dir=gSystem->OpenDirectory(INDIR);
  if(!dir)  {cout<<"No input directory "<<INDIR<<endl;exit(0);}

  TString fileName;
  vector<TString> DataFiles;
  while( (fileName=(gSystem->GetDirEntry(dir)))){
    if(fileName==TString(""))break;
    if(!fileName.Contains(".hipo"))continue;
    if(!fileName.Contains(RUNNO)&&RUNNO!=TString("GEMC"))continue;
    DataFiles.push_back(INDIR+fileName);
  }
  cout<<"Found "<<DataFiles.size()<<" files from run "<<RUNNO<<endl;
  //Calcualte number of farmjobs
  Int_t NJOBS=0;
  if(NFILES){
    NJOBS=DataFiles.size()/NFILES;
    if(DataFiles.size()%NFILES)
      NJOBS++; //extra job for remainder
  }
  else{ //run all files in 1 job
    NJOBS=1;
    NFILES=DataFiles.size();
  }
  cout<<"Going to run "<<NJOBS<<" jobs "<<endl;
  
  //Make a submit script
  TString submitscript=TString("doFarmRun")+RUNNO;
  gSystem->Exec(Form("rm %s",submitscript.Data()));
  gSystem->Exec(Form("touch %s",submitscript.Data()));
  gSystem->Exec(Form("chmod u+x %s",submitscript.Data()));

  //Add files to Farm script
  Int_t iFile=0;
  for(Int_t i=0;i<NJOBS;i++){
    
    TMacro script(FARMFILE);
    if(script.GetListOfLines()->GetEntries()==0)
      {cout<<"jsub template does not exist : "<<FARMFILE<<endl;exit(0);}

    //Add input files
    for(Int_t j=0;j<NFILES;j++)
      if(iFile<DataFiles.size())
	script.AddLine(DataFiles[iFile++]);

    //Add output dir
    Int_t iline=0;
    while(script.GetListOfLines()->At(iline)) {
      TString outline=script.GetListOfLines()->At(iline)->GetName();
      
      if(outline.Contains("OUTPUT_TEMPLATE")){
	outline=TString("OUTPUT_TEMPLATE: ")+OUTDIR;
	((TObjString*)script.GetListOfLines()->At(iline))->SetString(outline);
	if(i==0)cout<<outline<<endl;
	iline++;
  	continue;
      }
      if(outline.Contains("DISK_SPACE")){
 	outline=TString("DISK_SPACE: ")+Form("%d GB",5*NFILES);//5GB per file
	((TObjString*)script.GetListOfLines()->At(iline))->SetString(outline);
	if(i==0)cout<<outline<<endl;
	iline++;
	continue;	
	}
      if(outline.Contains("TIME:")){
 	outline=TString("TIME: ")+Form("%d",(Int_t)((Float_t)TperFILE*NFILES)/60);//total time expected
	if(i==0)cout<<outline<<endl;
	((TObjString*)script.GetListOfLines()->At(iline))->SetString(outline);
	iline++;
	continue;	
      }
      iline++;
      if(iline==script.GetListOfLines()->GetEntries())
	break;
    }
    
    TString jsubfile=TString("Run")+RUNNO+Form("_%d_",i)+gSystem->BaseName(FARMFILE);
    script.SaveSource(jsubfile);
    //Add to submit script
    gSystem->Exec(Form("echo 'jsub %s' >> %s",jsubfile.Data(),submitscript.Data()));
    
  }

  cout<<"Best to test 1 job first ! But to submit all you can run "<<submitscript<<endl;


}
