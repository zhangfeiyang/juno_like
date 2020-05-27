//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: B1DetectorConstruction.cc 75117 2013-10-28 09:38:37Z gcosmo $
//
/// \file B1DetectorConstruction.cc
/// \brief Implementation of the B1DetectorConstruction class

#include "B1DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4Ellipsoid.hh"
#include "G4Torus.hh"
#include "G4UnionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4SubtractionSolid.hh"
#include "G4RotationMatrix.hh"
#include "PMT.h"
#include "G4MaterialPropertiesTable.hh"
#include "G4Element.hh"
#include "B1PmtSD.hh"
#include "G4SDManager.hh"
#include "OpticalProperty.icc"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

	B1DetectorConstruction::B1DetectorConstruction()
: G4VUserDetectorConstruction(),
	fScoringVolume(0)
{ 
	pmtSD.Put(0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1DetectorConstruction::~B1DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B1DetectorConstruction::Construct()
{  

	DefineMaterials();
	G4bool checkOverlaps = true;

	//     
	// World
	//
	G4double world_sizeXYZ = 40*m;


	G4Box* solidWorld =    
		new G4Box("World",                       //its name
				0.5*world_sizeXYZ, 0.5*world_sizeXYZ, 0.5*world_sizeXYZ);     //its size

	G4LogicalVolume* logicWorld =                         
		new G4LogicalVolume(solidWorld,          //its solid
				vetoWater,           //its material
				"World");            //its name

	G4VPhysicalVolume* physWorld = 
		new G4PVPlacement(0,                     //no rotation
				G4ThreeVector(),       //at (0,0,0)
				logicWorld,            //its logical volume
				"World",               //its name
				0,                     //its mother  volume
				false,                 //no boolean operation
				0,                     //copy number
				checkOverlaps);        //overlaps checking

	double m_radLS = 17.7*m;
	double m_radAcrylic = 17.82*m;


	//--------define Acrylic ---------------------
	G4Sphere *solidAcrylic = new G4Sphere("sAcrylic",0,m_radAcrylic,0,360*deg,0,180*deg);
	G4LogicalVolume* logicAcrylic = new G4LogicalVolume(solidAcrylic,Acrylic,"lAcrylic");
	new G4PVPlacement(0, G4ThreeVector(),logicAcrylic,"pAcrylic",logicWorld,false,0);

	//--------define target ---------------------
	//	
	G4Sphere *solidTarget = new G4Sphere("sTarget",0,m_radLS,0,360*deg,0,180*deg);
	G4LogicalVolume* logicTarget = new G4LogicalVolume(solidTarget,LS,"lTarget");
	new G4PVPlacement(0, G4ThreeVector(),logicTarget,"pTarget",logicWorld,false,0);

	//--------define PMT ----------------------


	double r1t = 254*mm;
	double r4t = 78* mm;
	double r2t = 47*mm;
	double r3t = 50*mm;
	double r1zt = 182*mm;
	double h1t = 176*mm;
	double h2t = 42*mm;
	double h3t = 120*mm;
	double delta_torlerance = 0.01*mm;
	double m_H_1_2 = 219*mm;

	G4String solidname = "R12860";

	// * PART 1
	G4Ellipsoid* pmttube_solid_sphere = new G4Ellipsoid(
			solidname+"_1_Ellipsoid",
			r1t, // pxSemiAxis
			r1t, // pySemiAxis
			r1zt // pzSemiAxis
			);
	// * PART 2
	G4Tubs* pmttube_solid_tube = new G4Tubs(
			solidname+"_2_Torus",
			0*mm,  /* inner */ 
			r4t+delta_torlerance, /* pmt_r */ 
			h2t/2+delta_torlerance, /* part 2 h */
			0*deg, 
			360*deg);
	G4Torus* pmttube_solid_torus = new G4Torus(
			solidname+"_2_Torus",
			0*mm,  // R min
			r2t+delta_torlerance, // R max
			(r2t+r3t), // Swept Radius
			-0.01*deg,
			360.01*deg);
	G4SubtractionSolid* pmttube_solid_part2 = new G4SubtractionSolid(
			solidname+"_part2",
			pmttube_solid_tube,
			pmttube_solid_torus,
			0,
			G4ThreeVector(0,0,-h2t/2)
			);

	// * PART 3
	G4Tubs* pmttube_solid_end_tube = new G4Tubs(
			solidname+"_3_EndTube",
			0*mm,  /* inner */
			r3t+delta_torlerance, //21*cm/2, /* pmt_r */ 
			h3t/2+delta_torlerance, //30*cm/2, /* pmt_h */ 
			0*deg,
			360*deg);

	// Join

	// * PART 1 + 2
	G4UnionSolid* pmttube_solid_1_2 = new G4UnionSolid(
			solidname+"_1_2",
			pmttube_solid_sphere,
			pmttube_solid_part2,
			0,
			G4ThreeVector(0, 0, -(h1t+h2t/2))
			);
	//return pmttube_solid_1_2;
	// * PART 1+2 + 3
	G4UnionSolid* pmttube_solid_1_2_3 = new G4UnionSolid(
			solidname,
			pmttube_solid_1_2,
			pmttube_solid_end_tube,
			0,
			G4ThreeVector(0,0,
				-(m_H_1_2+h3t*0.50))
			);

	//logicPMT = new G4LogicalVolume(pmttube_solid_1_2_3,Pyrex,"lPMT");
	logicPMT = new G4LogicalVolume(pmttube_solid_1_2_3,Photocathode_mat_Ham20inch,"lPMT");


	for(int i=0;i<17739;i++){

		G4RotationMatrix rot;

		rot.rotateY(ry[i]*deg);
		rot.rotateZ(rz[i]*deg);

		G4ThreeVector pos(x[i],y[i],z[i]);
		G4Transform3D trans(rot,pos);

		//new G4PVPlacement(trans,logicPMT,"pPMT",logicWorld,false,i++,checkOverlaps);
		new G4PVPlacement(trans,logicPMT,"pPMT",logicWorld,false,i++,0);
	}

	fScoringVolume = logicTarget;	

	return physWorld;
}

void B1DetectorConstruction::ConstructSDandField(){

	if (pmtSD.Get() == 0){
		G4String name="/B1Det/pmtSD";
		B1PmtSD* aSD = new B1PmtSD(name);
		pmtSD.Put(aSD);
	}

	G4SDManager::GetSDMpointer()->AddNewDetector(pmtSD.Get());
	if(logicPMT){
		SetSensitiveDetector(logicPMT,pmtSD.Get());
	}

}

void  B1DetectorConstruction::DefineMaterials()
{

	//-- modify optical property -----
	
	for(int i=0;i<502;i++){
		GdLSABSLength[i] *= 77./26;	
	}
	
	for(int i=0;i<11;i++){
		GdLSRayLength[i] *= 27./46;
	}
	
	//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
	G4Element* H = new G4Element("Hydrogen", "H" , 1., 1.01*g/mole);
	G4Element* C = new G4Element("Carbon", "C" , 6., 12.01*g/mole);
	G4Element* O = new G4Element("Oxygen", "O", 8., 16.00*g/mole);
	G4Element* N = new G4Element("Nitrogen", "N", 7., 14.01*g/mole);
	G4Element* S = new G4Element("Sulfur", "S", 16., 32.066*g/mole);
	G4Element* K = new G4Element("Potassium", "K", 19., 39.0983*g/mole);
	G4Element* Si = new G4Element("Silicon", "Si", 14., 28.09*g/mole);
	G4Element* Na= new G4Element("Sodium", "Na", 11., 22.98977*g/mole);
	G4Element* B = new G4Element("Boron",  "B", 5, 10.811*g/mole);
	
	LS  = new G4Material("LS", 0.859*g/cm3, 5);
	LS->AddElement(C,  0.87924);
	LS->AddElement(H,  0.1201);
	LS->AddElement(O,  0.00034);
	LS->AddElement(N,  0.00027);
	LS->AddElement(S,  0.00005);

	G4MaterialPropertiesTable* LSMPT = new G4MaterialPropertiesTable();

	LSMPT->AddProperty("RINDEX",   GdLSRefIndexEnergy, GdLSRefIndex, 18);
	LSMPT->AddProperty("ABSLENGTH", GdLSABSEnergy, GdLSABSLength, 502);
	LSMPT->AddProperty("FASTCOMPONENT", GdLSComEnergy, GdLSFastComponent, 275);
	LSMPT->AddProperty("SLOWCOMPONENT", GdLSComEnergy, GdLSFastComponent, 275);
	LSMPT->AddProperty("REEMISSIONPROB", GdLSReemEnergy, GdLSReem, 28);
	LSMPT->AddProperty("RAYLEIGH", GdLSRayEnergy, GdLSRayLength, 11);
	LSMPT->AddProperty("SCINTILLATIONYIELD", component, GdLSLY,2);
	LSMPT->AddProperty("RESOLUTIONSCALE", component, GdLSResolutionScale,2);

	LSMPT->AddConstProperty("SCINTILLATIONYIELD", GdLSLY[0]);
	LSMPT->AddConstProperty("RESOLUTIONSCALE",GdLSResolutionScale[0]);
	LSMPT->AddConstProperty("FASTTIMECONSTANT",GdLSFastTimeConstant[0]);
	LSMPT->AddConstProperty("SLOWTIMECONSTANT",GdLSSlowTimeConstant[0]);
	LSMPT->AddConstProperty("YIELDRATIO",GdLSYieldRatio[0]);
	LSMPT->AddProperty("GammaFASTTIMECONSTANT", component, GdLSFastTimeConstant,2);
	LSMPT->AddProperty("GammaSLOWTIMECONSTANT", component, GdLSSlowTimeConstant,2);
	LSMPT->AddProperty("GammaYIELDRATIO", component, GdLSYieldRatio,2);

	// add fast/slow time constant for alpha
	LSMPT->AddProperty("AlphaFASTTIMECONSTANT", component, GdLSAlphaFastTimeConstant,2);
	LSMPT->AddProperty("AlphaSLOWTIMECONSTANT", component, GdLSAlphaSlowTimeConstant,2);
	LSMPT->AddProperty("AlphaYIELDRATIO", component, GdLSAlphaYieldRatio,2);

	// add fast/slow time constant for neutron
	LSMPT->AddProperty("NeutronFASTTIMECONSTANT", component, GdLSNeutronFastTimeConstant,2);
	LSMPT->AddProperty("NeutronSLOWTIMECONSTANT", component, GdLSNeutronSlowTimeConstant,2);
	LSMPT->AddProperty("NeutronYIELDRATIO", component, GdLSNeutronYieldRatio,2);

	// add fast/slow time constant for reemission
	LSMPT->AddProperty("ReemissionFASTTIMECONSTANT", component, GdLSReemissionFastTimeConstant,2);
	LSMPT->AddProperty("ReemissionSLOWTIMECONSTANT", component, GdLSReemissionSlowTimeConstant,2);
	LSMPT->AddProperty("ReemissionYIELDRATIO", component, GdLSReemissionYieldRatio,2);

	LSMPT->AddProperty("PPOABSLENGTH", GdLSABSEnergy, GdLSABSLength, 502);
	LSMPT->AddProperty("PPOREEMISSIONPROB", GdLSReemEnergy, GdLSReem, 28);
	LSMPT->AddProperty("PPOCOMPONENT", GdLSComEnergy, GdLSFastComponent, 275);
	LSMPT->AddProperty("PPOTIMECONSTANT", component, GdLSReemissionFastTimeConstant,2);

	LSMPT->AddProperty("bisMSBABSLENGTH", GdLSABSEnergy, GdLSABSLength, 502);
	LSMPT->AddProperty("bisMSBREEMISSIONPROB", GdLSReemEnergy, GdLSReem, 28);
	LSMPT->AddProperty("bisMSBCOMPONENT", GdLSComEnergy, GdLSFastComponent, 275);
	LSMPT->AddProperty("bisMSBTIMECONSTANT", component, GdLSReemissionFastTimeConstant,2);
	LS->SetMaterialPropertiesTable(LSMPT);




	// Acrylic
        Acrylic = new G4Material("Acrylic", 1.18*g/cm3, 3);
        Acrylic->AddElement(C, 0.59984);
        Acrylic->AddElement(H, 0.08055);
        Acrylic->AddElement(O, 0.31961);

        G4MaterialPropertiesTable* AcrylicMPT = new G4MaterialPropertiesTable();
        AcrylicMPT->AddProperty("ABSLENGTH", AcrylicAbsEnergy, AcrylicAbsLength, 9);
        AcrylicMPT->AddProperty("RAYLEIGH", AcrylicRayEnergy, AcrylicRayLength, 11);
        AcrylicMPT->AddProperty("RINDEX", AcrylicRefEnergy, AcrylicRefIndex, 18);

        Acrylic->SetMaterialPropertiesTable(AcrylicMPT);

	// veto Water
        vetoWater = new G4Material("vetoWater", 1*g/cm3, 2);
        vetoWater->AddElement(H,2);
        vetoWater->AddElement(O,1);
        G4MaterialPropertiesTable* vetoWaterMPT = new G4MaterialPropertiesTable();
        vetoWaterMPT->AddProperty("RINDEX", fPP_Water_RIN, fWaterRINDEX,36);
        vetoWaterMPT->AddProperty("ABSLENGTH", fPP_Water_ABS,fWaterABSORPTION, 316);
        vetoWater->SetMaterialPropertiesTable(vetoWaterMPT);
		
	// Photocathode_mat
        double density = 2.23*g/cm3;
        G4Material* SiO2 = new G4Material("SiO2", density, 2);
        SiO2->AddElement(Si, 1);
        SiO2->AddElement(O , 2);
		    G4Material* B2O2 = new G4Material("B2O2", density, 2);
        B2O2->AddElement(B,  2);
        B2O2->AddElement(O,  2);
        G4Material* Na2O = new G4Material("Na2O", density, 2);
        Na2O->AddElement(Na, 2);
        Na2O->AddElement(O,  1);
	
        Pyrex = new G4Material("Pyrex", density, 3);
        Pyrex->AddMaterial(SiO2, .80);
        Pyrex->AddMaterial(B2O2, .13);
        Pyrex->AddMaterial(Na2O, .07);
        G4MaterialPropertiesTable* PyrexMPT = new G4MaterialPropertiesTable();
        PyrexMPT->AddProperty("RINDEX", fPP_Pyrex, fPyrexRINDEX, 6);
        PyrexMPT->AddProperty("ABSLENGTH", fPP_PyrexABS, fPyrexABSORPTION, 9);
        Pyrex->SetMaterialPropertiesTable(PyrexMPT);

        Photocathode_mat_Ham20inch = new G4Material("photocathode_Ham20inch",5.*g/cm3,1);
        Photocathode_mat_Ham20inch->AddElement(K, 1);
        G4MaterialPropertiesTable* PhotocathodeMPT_Ham20inch = new G4MaterialPropertiesTable();
        PhotocathodeMPT_Ham20inch->AddProperty("RINDEX", fPP_PhC, fPhCRINDEX, 4);
        PhotocathodeMPT_Ham20inch->AddProperty("KINDEX", fPP_PhC, fPhCKINDEX, 4);
        PhotocathodeMPT_Ham20inch->AddProperty("REFLECTIVITY", fPP_PhC, fPhCREFLECTIVITY, 4);
				PhotocathodeMPT_Ham20inch->AddProperty("EFFICIENCY", fPP_PhCQE_1inch_20140620, fPhCEFFICIENCY_1inch_20140620, 43);
        //PhotocathodeMPT_Ham20inch->AddProperty("EFFICIENCY", fPP_PhCQE_Ham20inch, fPhCEFFICIENCY_Ham20inch, 43);
        //PhotocathodeMPT_Ham20inch->AddProperty("EFFICIENCY", fPP_PhCQE_Dynode20inch, fPhCEFFICIENCY_Dynode20inch, 43);
				double PhotocathodeMPTAbsEnergy[2] = {1*eV,15*eV};
				double PhotocathodeMPTAbsLength[2] = {0.01,0.02};
				//
				PhotocathodeMPT_Ham20inch->AddProperty("ABSLENGTH", PhotocathodeMPTAbsEnergy,PhotocathodeMPTAbsLength, 2);
        PhotocathodeMPT_Ham20inch->AddProperty("THICKNESS", fPosZ, fTHICKNESS, 2);
        Photocathode_mat_Ham20inch->SetMaterialPropertiesTable(PhotocathodeMPT_Ham20inch);
	
}
