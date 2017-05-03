To configure set HSANA variable to HaSpect directory

setenv HSANA /path_to_here/HaSpect

Copy the $HSANA/rootrc file to your top level directory.
If you already have a .rootrc file you can just copy the lines from within rootrc

cp $HSANA/rootrc ~/.rootrc

The purpose of the HASPECT data handling project is to :
  Use the ROOT data analysis framework to its full potential
   Build PROOF compatable code i.e. in accoradance with TSelector
   Integrate with RooFit analysis tools
 Create a user friendly enviroment
    Minimal configuration
    Provide useful examples and good documentation
    Provide minimal and optional additional code that can be used by users in their prefered manner
    Encourage users to optimise code (for speed, user friendliness and to do the best job)
  Be flexible and inclusive
    Code should be changed and improved as it develops and the goals become better defined
    Code should be written in a general manner, not specific to one experiment
  
