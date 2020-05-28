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
//
/// \file B1EventAction.cc
/// \brief Implementation of the B1EventAction class

#include "B1EventAction.hh"
#include "B1RunAction.hh"
#include "B1PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SDManager.hh"
#include "B1PmtHit.hh"
#include "g4root.hh"
#include <iostream>
#include "G4ThreeVector.hh"

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1EventAction::B1EventAction(B1RunAction* runAction)
: G4UserEventAction(),
  fRunAction(runAction),
  fEdep(0.),runAct(0),genAction(0)
{
  pmtCollID = -1;
} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B1EventAction::~B1EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1EventAction::BeginOfEventAction(const G4Event* evt)
{    
  if (!runAct)
    runAct =
      dynamic_cast<const B1RunAction*>
      (G4RunManager::GetRunManager()->GetUserRunAction());

  if (!genAction)
    genAction = dynamic_cast<const B1PrimaryGeneratorAction*>
      (G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());

  fEdep = 0.;
  G4SDManager *SDman = G4SDManager::GetSDMpointer();
  if(pmtCollID == -1){
    pmtCollID = SDman->GetCollectionID("pmtCollection");
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B1EventAction::EndOfEventAction(const G4Event* evt)
{   
  // accumulate statistics in run action

  G4AnalysisManager* man = G4AnalysisManager::Instance();

  B1PmtHitsCollection*   PHC = NULL;
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();

  if(HCE) {
    PHC = (B1PmtHitsCollection*)(HCE->GetHC(pmtCollID));
  }
  int P_hits = 0;
  if(PHC) {
    P_hits = PHC->entries();
		ofstream fout("data.out",ios::app);
		fout << P_hits << "\n";
		for(int i=0;i<P_hits;i++){
			double time = (*PHC)[i]->GetTime();
			G4ThreeVector pos = (*PHC)[i]->GetPos();
			double posX = pos.getX();
			double posY = pos.getY();
			double posZ = pos.getZ();
			fout << posX << "\t" << posY << "\t" << posZ << "\t" << time << "\n";
		}	
		fout.close();
  }
  fRunAction->AddEdep(fEdep);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
