#ifndef LINEARPOL_h
#define LINEARPOL_h

using namespace std;

enum {
  E_ID    = 0,
  ENERGY  = 1,
  ENH     = 2,
  ENHERR  = 3,
  ENHFIT  = 4,
  PFIT    = 5,
  PCOR    = 6,
  PCORERR = 7,
  PSMOOTH = 8,
  EDGE    = 9
};

enum {
  PARA1=0,
  PERP1=1,
  PARA2=2,
  PERP2=3,
  PARA3=4,
  PERP3=5
};

Double_t polTable[6][500][385][10];          // where its [PLANE][edge][E_id][field]
Int_t polTableN[6];                          // No of entries for para and perp
Char_t polFirstLines[6][500][250];           // to keep the 1st lines if needed (info from files)
  
 
double polcoh_edge_set=0;  // the intended setting of coherent edge position
int polE_e=0;           // electron energy (MeV)

int LoadPolTable(int PLANE, Char_t *PolTableList);
Double_t GetLinPol(Int_t PLANE, Double_t Edge, Double_t eg, Int_t poltype = PSMOOTH, Double_t lowThresh=0.2, Double_t highThresh=0.3);
void Read_run_info(int runno);
void SortPolarisations(TString runname);
#endif
