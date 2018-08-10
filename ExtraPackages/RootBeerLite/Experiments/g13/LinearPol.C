//Functions for finding the linear polarisation of an event
// you can load this into a ROOT analysis such as RootBeer via 
//  gROOT->LoadMacro("LinearPol.C++"); (in your main function for example)
// if you are compiling you will need a #include "LinearPol.C"
// After you move to a new file call :  SortPolarisations(inFile);
// Then just get the polarisation when required :
// for para GetLinPol(0, currentEdge, Egamma*1000., 8, 0.2, 0.3)
// for perp GetLinPol(1, currentEdge, Egamma*1000., 8, 0.2, 0.3)
// Note the polarisation tables are hardwired into this code 
// sprintf(pol_name_para, "/home/daria/g13/lists/pol/pol_list_%d_%.1f_para.dat", polE_e, polcoh_edge_set);
//  sprintf(pol_name_perp, "/home/daria/g13/lists/pol/pol_list_%d_%.1f_perp.dat", polE_e, polcoh_edge_set);
 
#include <fstream> 
#include <iostream> 
#include <sstream>   
#include <TString.h>
#include <TSystem.h>
#include "LinearPol.h"



//
int LoadPolTable(int PLANE, const Char_t *PolTableList){

  FILE *fplist, *fpfile;              //file pointers for filelist and each file therein
  Char_t lline[250];                 //for reading in lines from filelist
  Char_t fline[250];                 //for reading in lines from file
  Char_t filename[250];              //file
  Int_t  fcount = 0;                 //counter for no of files read in
  Int_t  chancount = 0;              //counter for no of channels read in
  Int_t  eid = 0;
  Double_t Edge = 0.0;
  

  /***** Open list of files ********/

  if((fplist = fopen(PolTableList,"r")) == NULL){                   //open filelist
    cerr << "Error Couldn't open file: " << PolTableList << endl;
    return -1;
  }
  
  /****** Read in each file and open it one by one *****/
  
  fcount = 0;   // counter for number of files to be read in
  
  // for each file in the list:
  
  while (fgets(lline, 240, fplist) != NULL){                  // reads in each line from fplist (up to 240 char) into lline
            
    sscanf(lline,"%s",filename);                              // writes it to filename. Need this step, else think fgets apends space to end of name

    //    cout << "opening " << filename << "   " << endl;
    
    if ((fpfile = fopen(filename,"r")) == NULL){                 // open file, proceed to error message if it's not openable
      cerr << "Error Couldn't open file: " << filename << endl;
      return -1;
    }
    
    fgets(polFirstLines[PLANE][fcount], 240, fpfile );   //save the 1st line of each file
    
    //scan the bit after the last "_" in the filename to get the edge energy
    sscanf(strrchr(filename,'_')+1,"%lg",&polTable[PLANE][fcount][0][EDGE]);   // strrchr returns pointer to last occuring char _ in filename
    
    //  cout << " Edge energy: " << polTable[PLANE][fcount][0][EDGE] << endl;
    
    chancount = 0;                                           
    
    while ((fgets(fline, 240, fpfile)) != NULL){             // read in the polarisation table line by line
      
      if ((fline[0] == '*') || (fline[0] == '#')) continue;  // skip comments
      
      sscanf(fline, "%d", &eid);                             // first get the E_ID
      
      //  cout << "\t E_id read in: " << eid << endl;
      
      sscanf(fline,"%*d%lg%lg%lg%lg%lg%lg%lg%lg",
	     &polTable[PLANE][fcount][eid][ENERGY],
	     &polTable[PLANE][fcount][eid][ENH],
	     &polTable[PLANE][fcount][eid][ENHERR],
	     &polTable[PLANE][fcount][eid][ENHFIT],
	     &polTable[PLANE][fcount][eid][PFIT],
	     &polTable[PLANE][fcount][eid][PCOR],
	     &polTable[PLANE][fcount][eid][PCORERR],
	     &polTable[PLANE][fcount][eid][PSMOOTH]);
      
      chancount++;
    }
    
    fclose(fpfile); //close the file
    
    //   for (int i=0; i < chancount+1; i++){
    //    cout << "Info read in for " << i << " channel: " << polTable[PLANE][fcount][i][ENERGY] << ", " << polTable[PLANE][fcount][i][ENH] << ", " << polTable[PLANE][fcount][i][ENHERR] << ", " << polTable[PLANE][fcount][i][ENHFIT] << ", " << polTable[PLANE][fcount][i][PFIT] << ", " << polTable[PLANE][fcount][i][PCOR] << ", " << polTable[PLANE][fcount][i][PCORERR] << ", " << polTable[PLANE][fcount][i][PSMOOTH] << ", " << polTable[PLANE][fcount][i][EDGE] << endl;
    //   }
    
    if (chancount != 384){
      cerr << "Should be 384 lines in " << filename << " - only got " << chancount << endl;
      return -1;
    }
    
    polTableN[PLANE]++;   // counts the number of pol files read in for that plane
    
    fcount++;
  }
  
  fclose(fplist);
  
  return(0);
}


//function to call event by event to get the degree of linear polarisation
//requires that valid polarisation tables are loaded

Double_t GetLinPol(Int_t PLANE, Double_t Edge, Double_t eg, Int_t poltype, Double_t lowThresh, Double_t highThresh){

  // Get polarization based on ephoton energy and edge position
  
  Int_t eIndex = 0;
  Double_t POL = -1.0;
  Int_t eid = 0;
  // Check that edge is in range of tables:

  if ((Edge < polTable[PLANE][0][0][EDGE]) || (Edge > polTable[PLANE][polTableN[PLANE]-1][0][EDGE])){
    // cout << "Edge out of range of pol table!" << polTable[PLANE][0][0][EDGE]<<" "<<Edge<<" "<<polTable[PLANE][polTableN[PLANE]-1][0][EDGE]<<endl;
    return -1.0;
  }  

  // Find index (corresponding to the edge) for the correct array:
  
  for (eIndex = 0; eIndex < polTableN[PLANE]; eIndex++){
    
    if(polTable[PLANE][eIndex][0][EDGE] >= Edge) break;
  }
  
  //  cout << "Edge index = " << eIndex << ", edge: " << polTable[PLANE][eIndex][0][EDGE] << endl;
  
  // Find eid:
  
  for (eid = 1; eid <= 384; eid++){
    if(polTable[PLANE][eIndex][eid][ENERGY] <= eg) break;
  }

  if (eg >= (polTable[PLANE][eIndex][eid][ENERGY] + polTable[PLANE][eIndex][eid-1][ENERGY]) / 2.) eid = eid-1;
  
  //   cout << "eid = " << eid << endl;

  // Get polarisation:

  POL = polTable[PLANE][eIndex][eid][poltype];
  
  // cout << "Energy value: " << polTable[PLANE][eIndex][eid][ENERGY] << ", Edge: " << Edge << ", Pol: " << POL << endl;
  
  if ((polTable[PLANE][eIndex][eid][ENERGY] < Edge) && (POL < lowThresh)) POL = -1.0;
  if ((polTable[PLANE][eIndex][eid][ENERGY] > Edge) && (POL < highThresh)) POL = -1.0;
   
  return POL;
}


//for each run find the nominal coherent edge position
//polE_e and polcoh_edge_set need to be global
void Read_run_info(int runno){
  
  if ((runno >= 54042) && (runno <= 54145)){ 
    
    polE_e = 3309;
    polcoh_edge_set = 1.3;
  }

  else if ((runno >= 54162) && (runno <= 54228)){ 
    
    polE_e = 3920;
    polcoh_edge_set = 1.3;
  }

  else if ((runno >= 54229) && (runno <= 54323)){
    
    polE_e = 4756; 
    polcoh_edge_set = 1.7;
  }

  else if ((runno >= 54324) && (runno <= 54502)){   
    
    polE_e = 5052;    
    polcoh_edge_set = 1.9;
  }

  else if ((runno >= 54503) && (runno <= 54600)){
    
    polE_e = 5052;   
    polcoh_edge_set = 2.1;
  }

  else if ((runno >= 54607) && (runno <= 54640)){   
    
    polE_e = 4199;  
    polcoh_edge_set = 1.3;
  }

  else if ((runno >= 54641) && (runno <= 54705)){  
    
    polE_e = 4482; 
    polcoh_edge_set = 1.5;
  }

  else if ((runno >= 54707) && (runno <= 54715)){ 
    
    polE_e = 5166;    
    polcoh_edge_set = 2.1;
  }

  else if (runno == 54716){ 
    
    polE_e = 5166;
    polcoh_edge_set = 2.3;
  }

  else if ((runno >= 54720) && (runno <= 54770)){   
    
    polE_e = 5166;
    polcoh_edge_set = 2.1;
  }

  else if ((runno >= 54784) && (runno <= 55001)){
    
    polE_e = 5166;  
    polcoh_edge_set = 2.3;
  }

  else if ((runno >= 55011) && (runno <= 55047)){  
    
    polE_e = 4072;       
    polcoh_edge_set = 1.5;
  }

  else if ((runno >= 55048) && (runno <= 55125)){
    
    polE_e = 4072;   
    polcoh_edge_set = 1.7;
  }

  else if ((runno >= 55126) && (runno <= 55140)){   
    
    polE_e = 4072;    
    polcoh_edge_set = 1.5;
  }

  else {
    polE_e = 0;
    polcoh_edge_set = 0.;
    cout << "Can't determine electron energy or coherent edge position setting from run number " << runno << endl; 
  }
 
}

//Function added by dglazier to setup linear polarisation tables from run name
//Adapted from Daria's TSelector code
void SortPolarisations(TString runname){
  //Get the run number from the filename
  //e.g. /w/work12/jlab/g13/pass1/K0Lskim/Run_055048.root we get the 6 digits before .root
  TString srunNo(runname(runname.Index(".root")-6,6));
  Int_t runNo=srunNo.Atoi();
  Read_run_info(runNo);

  for (int i=0; i<6; i++) polTableN[i] = 0;
 
   
  TString G13=gSystem->Getenv("G13");
  if(G13==TString("")) cout<<"ERROR : LinearPol.C G13 enviroment variable not set"<<endl;
  TString parafile=Form("%s/pol/pol_list_%d_%.1f_para.dat",G13.Data(),polE_e, polcoh_edge_set);
  TString perpfile=Form("%s/pol/pol_list_%d_%.1f_perp.dat",G13.Data(),polE_e, polcoh_edge_set);
  // char pol_name_para[200];
  //char pol_name_perp[200];
  LoadPolTable(PARA1, parafile.Data());
  LoadPolTable(PERP1, perpfile.Data());

  // sprintf(pol_name_para, "/home/dglazier/dglazier2/Research/Jlab/g13/daria/pol/pol_list_%d_%.1f_para.dat", polE_e, polcoh_edge_set);
  // sprintf(pol_name_perp, "/home/dglazier/dglazier2/Research/Jlab/g13/daria/pol/pol_list_%d_%.1f_perp.dat", polE_e, polcoh_edge_set);
  
  // LoadPolTable(PARA1, pol_name_para);
  // LoadPolTable(PERP1, pol_name_perp);


}
