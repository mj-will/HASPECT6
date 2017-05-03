{
  gBenchmark->Start("time");
   std::clock_t start;
   double duration;
   start = std::clock();
   //Open input file
   TFile* file=new TFile("/home/dglazier/Dropbox/NoNPC/rout_clas12_000809_a00902.root");
   // TTree* tree=(TTree*)file->Get("hipo2root");
   //TChain *tree=new TChain("hipo2root","test");
   //tree->Add("/home/dglazier/Dropbox/NoNPC/out_clas12_000809_a00902.root");
   
   //Create ROOT 6 tree reader
   TTreeReader fReader("hipo2root",file);
   TTreeReaderArray<int> RECHB_Particle_pid = {fReader, "RECHB_Particle_pid"};
   // TTreeReaderArray<int> RECHB_Particle_charge = {fReader, "RECHB_Particle_charge"};
   // TTreeReaderArray<int> RECHB_Particle_status = {fReader, "RECHB_Particle_status"};
   TTreeReaderArray<float> RECHB_Particle_px = {fReader, "RECHB_Particle_px"};
  TTreeReaderArray<float> RECHB_Particle_py = {fReader, "RECHB_Particle_py"};
  TTreeReaderArray<float> RECHB_Particle_pz = {fReader, "RECHB_Particle_pz"};
  TTreeReaderArray<float> RECHB_Particle_vx = {fReader, "RECHB_Particle_vx"};
  TTreeReaderArray<float> RECHB_Particle_vy = {fReader, "RECHB_Particle_vy"};
  TTreeReaderArray<float> RECHB_Particle_vz = {fReader, "RECHB_Particle_vz"};
  TTreeReaderArray<float> RECHB_Particle_mass = {fReader, "RECHB_Particle_mass"};
  // TTreeReaderArray<float> RECHB_Particle_beta = {fReader, "RECHB_Particle_beta"};
  // TTreeReaderArray<float> RECHB_Particle_chi2pid = {fReader, "RECHB_Particle_chi2pid"};
  //fReader.SetTree(tree);
  Int_t counter=0;
  while(fReader.Next()){
    for(UInt_t i=0;i<RECHB_Particle_pid.GetSize();i++){
      //cout<<RECHB_Particle_px[i]<<endl;
      RECHB_Particle_pid[i];
      RECHB_Particle_px[i];
      RECHB_Particle_py[i];
      RECHB_Particle_pz[i];
      RECHB_Particle_vx[i];
      RECHB_Particle_vy[i];
      RECHB_Particle_vz[i];
      RECHB_Particle_mass[i];
      counter++;
     }
  }
  cout<<"counter "<<counter<<endl;
  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
   cout<<"duration "<<duration<<endl;
  gBenchmark->Stop("time");
  gBenchmark->Print("time");

}
