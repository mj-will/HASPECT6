//run with root --THSSkeleton.C CreateMyFinalState.C
{
  THSSkeleton* sk=new THSSkeleton();
  //Give your project a name
  sk->SetFinalState("Test",kTRUE);  //creating the final state class 
  //Set the detected particle combinations you will analyse
  sk->SetFinalStateTopo("Beam:neutron:gamma:gamma");
  //Set the actual final state particles of the reaction
  //These will just be used as the data member names, for example
  sk->SetFinalStateParts("Beam:gamma,Neutron:neutron,Gamma1:gamma,Gamma2:gamma");
   //define any parent particles that may have decayed, for example
  sk->SetFinalStateParents("Pi0:pi0;Gamma1;Gamma2");
  //Uncomment for CLAS12 functionality
  //sk->SetCLAS12();
 //Make some code
  sk->CreateMyFinalState();
}
