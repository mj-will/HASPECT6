#include <iostream>
#include <string>
#include "TString.h"
#include "TH1F.h"
#include "TFile.h"
#include "TDatabasePDG.h"
#include "IUAmpTools/AmplitudeManager.h"
#include "IUAmpTools/ConfigFileParser.h"
#include "IUAmpTools/ConfigurationInfo.h"
#include "IUAmpTools/AmpToolsInterface.h"
#include "HSDataReader.h"
#include "HSDataWriter.h"
#include "BreitWigner.h"
#include "TwoPSAngles.h"
using namespace std;

int main(int argc, char** argv){


    // ************************
    // usage
    // ************************

  cout << endl << " *** Generating Events According to Amplitudes *** " << endl << endl;

  if (argc <= 2){
    cout << "Usage:" << endl << endl;
    cout << "\tpseudoData <config file name> <output file name>" << endl << endl;
    return 0;
  }


    // ************************
    // parse the command line parameters
    // ************************

  string cfgname(argv[1]);
  string outfilename(argv[2]);

  cout << "Config file name = " << cfgname << endl << endl;
  cout << "Output file name = " << outfilename << endl;


    // ************************
    // parse the config file
    // ************************

  ConfigFileParser parser(cfgname);
  ConfigurationInfo* cfgInfo = parser.getConfigurationInfo();
  cfgInfo->display();


    // ************************
    // AmpToolsInterface
    // ************************

  AmpToolsInterface::registerAmplitude(BreitWigner());
  AmpToolsInterface::registerAmplitude(TwoPSAngles());
  AmpToolsInterface::registerDataReader(HSDataReader());

  AmpToolsInterface ATI(cfgInfo);

  HSDataReader* dataReader = dynamic_cast<HSDataReader*>(ATI.dataReader(cfgInfo->reactionList()[0]->reactionName()));

  ATI.loadEvents(dataReader);

  HSDataWriter* dataWriter = new HSDataWriter(outfilename);
  dataWriter->setTree(dataReader->getTree());

 // ************************
    // calculate intensities for all events
    // ************************

  cout << "calculating intensities..." << endl;
  double maxIntensity = 1.2 * ATI.processEvents(cfgInfo->reactionList()[0]->reactionName());
  cout << "... finished calculating intensities" << endl;


    // ************************
    // loop over all events again and do accept/reject
    // ************************

  cout << "doing accept/reject..." << endl;
  dataReader->resetSource();//back to first event
  for (int i = 0; i < dataReader->numEvents(); i++){
    dataReader->getTreeEntry(i);//get the event kinematics
    double intensity = ATI.intensity(i); 
    double rndm = drand48() * maxIntensity;
    Kinematics* kin=ATI.kinematics(i);
    //  ATI.printKinematics(cfgInfo->reactionList()[0]->reactionName(),kin);
    //ATI.printAmplitudes(cfgInfo->reactionList()[0]->reactionName(),kin);
    //   ATI.printIntensity(cfgInfo->reactionList()[0]->reactionName(),kin);
    //cout<<intensity<<endl;
    if (intensity > rndm){
      dataWriter->writeEvent();
   
    }
  
  }
  cout << "... finished doing accept/reject" << endl;

  cout << "KEPT " << dataWriter->eventCounter() << "  events" << endl;

  delete dataWriter;

}
