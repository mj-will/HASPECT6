import org.jlab.io.hipo.*;
import org.jlab.io.base.*;
import org.jlab.clas.detector.*;
import org.jlab.clas.physics.*;
import org.jlab.clas.pdg.PDGDatabase;
import org.jlab.clas.pdg.PDGParticle;


String filename="ep_eppi0";
HipoDataSource reader = new HipoDataSource();
reader.open("/auto_data/fiber7/sdiehl/conversion/ppi0/"+filename+".hipo");

/// Open text file for storing "tree"

File treeFile = new File("/auto_data/fiber7/sdiehl/conversion/ppi0/"+filename+".lund");

// define countung variables:

int eventCounter = 0;

int run=0;
double weight;

int gen=0;
int rec=0;
int recCLAS=0;
int recFT=0;
int recminus=0;
int reczero=0;
int recplus=0;

int rec1=0;
int rec2=0;
int rec3=0;
int recx=0;


/// Loop over all events:

while(reader.hasEvent())
{
	// get new event:

	DataEvent event = reader.getNextEvent();
	DetectorEvent detectorEvent = DetectorData.readDetectorEvent(event,"REC::Particle","REC::Detector");
	PhysicsEvent physEvent = detectorEvent.getPhysicsEvent();

	// reset counting variables:

	gen=0;
	rec=0;
	recCLAS=0;
	recFT=0;
	recminus=0;
	reczero=0;
	recplus=0;


	// read data from the forward tagger bank:

	// initialize the number of entries:

	if(event.hasBank("FT::particles")) {
	    DataBank bankFT = event.getBank("FT::particles");
            recFT = bankFT.rows();
	}

	// define variables:

	def id = new Short[recFT];
	def charge = new Byte[recFT];
	def pid = new Short[recFT];
	def energy = new Float[recFT];
	def mass = new Float[recFT];
	def px = new Float[recFT];
	def py = new Float[recFT];
	def pz = new Float[recFT];
	def time_FT = new Float[recFT];
	def track_mass_FT = new Float[recFT];
	def momentum = new Float[recFT];

	// initialize variables:

	for(int i=0; i<recFT;i++) {
	  id[i] = 0;
	  charge[i] = 0;
	  pid[i] = 0;
	  energy[i] = 0.0;
          px[i] = 0.0;
	  py[i] = 0.0;
	  pz[i] = 0.0;
	  mass[i] = 0.0;
	  track_mass_FT[i] = 0.0;
          time_FT[i] = 0.0;
	  momentum[i]=0.0;
	}

	// read variables from bank if bank exists:

	if(event.hasBank("FT::particles")) {
                    DataBank bankFT = event.getBank("FT::particles");
                    for(int i=0; i<recFT;i++) {
			id[i] = bankFT.getShort("id",i);
			charge[i] = bankFT.getByte("charge",i);
			energy[i] = bankFT.getFloat("energy",i);
			pid[i] = 0;
			momentum[i]=1.0;
			
			// if PID and momentum calculatyion should be done 
                        // already at this point, uncomment the following section:
			// else a position vector (IpI = 1) will be forwarded as momentum
			  
			//if(charge[i] == -1){
			//  pid[i] = 11;                  // electron (this is assumed every time for the FT)
			//  momentum[i]= energy[i];       // for E > 0.5 GeV (FT range) the electron mass can be ignored 
			//}
			//if(charge[i] == 0){
			//  pid[i] = 22;                  // gamma (this is assumed every time for the FT)
			//  momentum[i] = energy[i];
			//}
			//if(charge[i] == 1){
			//  pid[i] = 0;        // (can not be clearly identified)	
			//  momentum[i] = 1;   // has to be computed from the energy in the analysis
			//}

			px[i] = bankFT.getFloat("cx",i)*momentum[i];
			py[i] = bankFT.getFloat("cy",i)*momentum[i];
			pz[i] = bankFT.getFloat("cz",i)*momentum[i];

			mass[i] = 0;
			track_mass_FT[i] = 0;
                        time_FT[i] = bankFT.getFloat("time",i);
                    }
	  }


	// read additional data (track mass) from the reconstructed particle bank:

	// initialize the number of entries:

	if(event.hasBank("REC::Particle")) {
                    DataBank bankREC = event.getBank("REC::Particle");
		    recCLAS = bankREC.rows();
	}

	// define variables:

	def track_mass_CLAS = new Float[recCLAS];
	def time_CLAS = new Float[recCLAS];

	// initialize variables:

	for(int i=0; i<recCLAS;i++) {
	  track_mass_CLAS[i] = 0.0;
	  time_CLAS[i] = 0.0;
        }

	// read variables from bank if bank exists:

	if(event.hasBank("REC::Particle")) {
                    DataBank bankREC = event.getBank("REC::Particle");
                    for(int i=0; i<recCLAS;i++) {
			track_mass_CLAS[i] = bankREC.getFloat("mass",i);
			time_CLAS[i] = 0.0;
                    }
	  }


	// read the runnumber from the RUN::config bank:

        DataBank bankRUN = event.getBank("RUN::config");
	run = bankRUN.getInt("run",0);

	weight = 1;


        // write output for genrated particles:

	gen = physEvent.countGenerated();

	treeFile << gen <<" 0 "<< eventCounter <<" 0"<<" 0"<<" 0"<<" 0"<<" 0"<<" 0 "<< weight <<"\n";

	for(int i=0;i<gen;i++){
	    Particle partG = physEvent.getGeneratedParticle(i);
	    if(partG) treeFile << i+1 << partG.toLundString() << "   0.0  " << "   0.0" << "\n";
	}


	// write output for reconstructed particles:

	recCLAS = physEvent.count();
	rec = recCLAS + recFT;

	treeFile << rec << " " << run << " " << eventCounter <<" 0"<<" 0"<<" 0"<<" 0"<<" 0"<<" 0 "<< weight <<"\n";

	for(int i=0;i<recCLAS;i++){
	    Particle part = physEvent.getParticle(i);
	    if(part) treeFile << i+1 << part.toLundString() << "    " << track_mass_CLAS[i] << "    " << time_CLAS[i] << "    100" << "\n";
	 }
	for(int i=0;i<recFT;i++){
	 if(recFT > 0) treeFile << recCLAS+1+i << "  " << charge[i] << ".    1     " << pid[i] << "   0  " << " 0  " << px[i] << "   " << py[i] << "   " << pz[i] << "   " << energy[i] << "   " << mass[i] <<  "  0.0000  " << "  0.0000  " << "  0.0000     " << track_mass_FT[i] << "     " << time_FT[i] << "    300" << "\n";
	 }

	 
	// count particles by charge
   
	 recminus = physEvent.countByCharge(-1);
	 for(int i=0;i<recFT;i++){
	   if(recFT > 0 && charge[i] == -1) recminus++;
	 }
	 reczero = physEvent.countByCharge(0);
         if(recFT > 0){ 
	   for(int i=0;i<recFT;i++){
	     if(charge[i] == 0) reczero++;
	   }
	 }
	 recplus = physEvent.countByCharge(1);
	 for(int i=0;i<recFT;i++){
	   if(recFT > 0 && charge[i] == 1) recplus++;
	 }


   if(eventCounter%100 == 0){
      println "Analyzed event number " + eventCounter;
      println "Current event contains " + gen + " generated particles and " + rec + " reconstructed particles.";
      println recminus + " negative, " + reczero + " neutral, " + recplus + " positive.";
   }

   eventCounter++;

   if(rec == 1){ rec1++;}
   if(rec == 2){ rec2++;}
   if(rec == 3){ rec3++;}
   if(rec > 3){ recx++;}

}


println "Done and Analyzed " + eventCounter + " events... ";
println "Events with 1 reconstructed particle " + rec1;
println "Events with 2 reconstructed particles " + rec2;
println "Events with 3 reconstructed particles " + rec3;
println "Events with more than 3 reconstructed particles " + recx;
