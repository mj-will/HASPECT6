//run with root --THSSkeleton.C CreateMyFinalState.C
{
  THSSkeleton* sk=new THSSkeleton();
  //Give your project a name
  sk->SetFinalState("K0L",kTRUE);  //creating the final state class 
  //Set the detected particle combinations you will analyse
  sk->SetFinalStateTopo("Beam:proton:pi+:pi-:pi-,Beam:proton:pi+:pi-:pi-:proton");
  //Set the actual final state particles of the reaction
  //These will just be used as the data member names, for example
  sk->SetFinalStateParts("Beam:gamma,Proton:proton,Pip:pi+,PimL:pi-,PimK:pi-,Spectator:proton");
   //define any parent particles that may have decayed, for example
  sk->SetFinalStateParents("Lambda:Lambda0;Proton;PimL,K0:K0;Pip;PimK");
  //Uncomment for CLAS12 functionality
  //sk->SetCLAS12();
 //Make some code
  sk->CreateMyFinalState();
}
