//run with root --THSSkeleton.C CreateMyFinalState.C
{
  THSSkeleton* sk=new THSSkeleton();
  //Give your project a name
  sk->SetFinalState("ep_omegap");  //creating the project class with perms
  //Set the detected particle combinations you will analyse
  sk->SetFinalStateTopo("pi+:pi-:gamma:gamma:proton:e-,pi+:pi-:proton:e-");
  //Set the actual final state particles of the reaction
  //These will just be used as the data member names
  sk->SetFinalStateParts("Electron:e-,Proton:proton,Pip:pi+,Pim:pi-,Gamma1:gamma,Gamma2:gamma");
  //define any parent particles that may have decayed
  sk->SetFinalStateParents("Pi0:pi0;Gamma1;Gamma2,Omega:omega;Pip;Pim;Pi0");
  //Make some code
  sk->CreateMyFinalState();
}
