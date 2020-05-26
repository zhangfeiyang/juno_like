{
	ifstream fin("data.out");
	
	int nPhotons;
	float GlobalPosX[100000];
	float GlobalPosY[100000];
	float GlobalPosZ[100000];
	float time[100000];
	TFile *file = new TFile("data.root","recreate");
	
	TTree *tree = new TTree("tree","");
	tree->Branch("nPhotons",&nPhotons,"nPhotons/I");
	tree->Branch("GlobalPosX",GlobalPosX,"GlobalPosX[nPhotons]/F");
	tree->Branch("GlobalPosY",GlobalPosY,"GlobalPosY[nPhotons]/F");
	tree->Branch("GlobalPosZ",GlobalPosZ,"GlobalPosZ[nPhotons]/F");
	tree->Branch("time",time,"time[nPhotons]/F");

	//while(!fin.eof()){
		fin>>nPhotons;
		for(int i=0;i<nPhotons;i++){
			fin>>GlobalPosX[i]>>GlobalPosY[i]>>GlobalPosZ[i]>>time[i];
		}
		tree->Fill();
	//}
	tree->Write();	
	file->Close();
}
