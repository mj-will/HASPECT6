# Guide to using THS Multivariate analysis

### FinalState setup

The finalstate class outlines the particles involved in the reaction and the variables that will be used for the MVA analysis. The included examples shows the rough layout for this class.


For the MVA analysis you must:
* Pass the pointers to the THSParticles that will be used
* Pass the variables that will used for each particle, either by setting a default set or individually specifying them (NOTE: these should be floats if they are to used for the MVA)
* Add the methods to train in the form of method classes
* Add any "splits", e.g. train different classifiers for each topology.
** If using pointers, they will need to be updated for application in the `SetApplication`
* Define any variables to be output in the final tree e.g. physics variables
* Define the output trees for training and application

There a various other functions that should be left as are.


### RunFS<finalstate> setup

The RunFS script is used to run the MVA analysis. The included example shows versions for training and application.
