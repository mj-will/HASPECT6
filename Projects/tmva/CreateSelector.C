/** \file CreateSelector.C
 * 
 * Macro to generate a TSelector
 * using the THSSkeleton class
 * 
 * Usage:
 * root --hssel CreateSelector.C
 * 
 */

void CreateSelector(){
	
	THSSkeleton* sk = new THSSkeleton();
	
	sk->SetHisto();   //Going to make histograms
	sk->SetNewTree();   //Going to make new tree
	
	sk->SetFinalState("THS2pi"); //Give final state class name
	
	sk->CreateSelector("process","/home/dglazier/clas12data/gemc/May29_18/rho10_tm1_sm1_5.3.3/out_gemc_39442108.farmpbs14_rho10_tm1_sm1.hipo.root","HSParticles"); //create selector with all specifications
	
};
