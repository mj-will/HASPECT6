/**
	\class THSCLAS12DeltaTime
	A derived class of THSCuts used to apply timing cuts to 
	a given THSParticle.

	This can be configured (i.e. cut width for each detector) via
	SetCut Methods :

	    fCuts.SetElCut(1,1); //Just FT,FD
	    fCuts.SetPionPCut(0,1,0.,0);//FT,FDTOF,CD,FDCAL
	    fCuts.SetPionMCut(0,1,0.,0);//FT,FDTOF,CD,FDCAL
	    fCuts.SetProtCut(0,1,0.5,0);//FT,FDTOF,CD,FDCAL
	    fCuts.SetGammaCut(4,4,0,4);//FT,FDTOF,CD,FDCAL

	In a THSFinalState class this can be called in FileStart()

	to see if a particle passes a cut supply the address of a THSParticle
	
	    fCuts.ElCut(&fElectron); //Is fElectron an electron?

        So this can be used in an if statement (for example in a THSFinalState::Topo_X() function
	    if(!fCuts.ElCut(&fElectron)){fGoodEvent=kFALSE;return;}
	    if(!fCuts.ProtCut(&fProton)){fGoodEvent=kFALSE;return;}
	    if(!fCuts.PionPCut(&fPip)){fGoodEvent=kFALSE;return;}
	    if(!fCuts.PionMCut(&fPim)){fGoodEvent=kFALSE;return;}



*/
#include "THSCLAS12DeltaTime.h"

