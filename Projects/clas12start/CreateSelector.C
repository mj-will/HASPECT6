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
	
	sk->SetFinalState("THSep_omegap"); //Give final state class name
	
	sk->CreateSelector("Histo","data/out_gemc_32975712.farmpbs14_t-1_s1.root","HSParticles"); //create selector with all specifications
	
};
