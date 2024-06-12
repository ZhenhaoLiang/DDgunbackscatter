/// \file SteppingAction.cc
/// \brief Implementation of the SteppingAction class

#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "g4root.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(EventAction* eventAction)
: fEventAction(eventAction)
{}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    if (step->GetTrack()->GetVolume()->GetName() == "DetectorTub") //Only neutron pass Detector would be recorded.
    {
        auto analysisManager = G4AnalysisManager::Instance();
        G4float px = step->GetPreStepPoint()->GetPosition().x();
        G4float py = step->GetPreStepPoint()->GetPosition().y();
        G4float pz = step->GetPreStepPoint()->GetPosition().z();
        G4float xp = step->GetPostStepPoint()->GetPosition().x();
        G4float yp = step->GetPostStepPoint()->GetPosition().y();
        G4float zp = step->GetPostStepPoint()->GetPosition().z();


        G4String particlename = step->GetTrack()->GetDefinition()->GetParticleName();
        
        G4double energy = 1000*  step->GetPreStepPoint()->GetKineticEnergy();
        G4double dE = 1000* step->GetTotalEnergyDeposit();

        analysisManager->FillNtupleDColumn(0, energy);
        analysisManager->FillNtupleDColumn(1, px);
        analysisManager->FillNtupleDColumn(2, py);
        analysisManager->FillNtupleDColumn(3, pz);
        analysisManager->FillNtupleDColumn(4, xp);
        analysisManager->FillNtupleDColumn(5, yp);
        analysisManager->FillNtupleDColumn(6, zp);
        analysisManager->FillNtupleSColumn(7, particlename);
        analysisManager->FillNtupleDColumn(8,G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID());
        analysisManager->FillNtupleDColumn(9, step->GetTrack()->GetTrackID());
        analysisManager->FillNtupleDColumn(10, step->GetTrack()->GetParentID());
        analysisManager->FillNtupleDColumn(11, dE);
        analysisManager->AddNtupleRow();
    }
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

