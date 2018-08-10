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
	
	sk->SetFinalState("THSK0L"); //Give final state class name
	
	sk->CreateSelector("HistParticles","/w/work1/jlab/hallb/g13/K0L18/K0LSim_11_23/DSTJob14659644.farmpbs14.root","HSParticles"); //create selector with all specifications
	
};
