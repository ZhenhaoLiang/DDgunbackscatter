/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include <G4Tubs.hh>
#include <G4RotationMatrix.hh>
#include <G4ThreeVector.hh>
#include <G4UnionSolid.hh>
#include <G4SubtractionSolid.hh>
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include <G4VisAttributes.hh>

#define pi 3.14159265359

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
: G4VUserDetectorConstruction()
{
  DefineMaterial();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::DefineMaterial()
{
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();
  Air = nist->FindOrBuildMaterial("G4_AIR");
  Water = nist->FindOrBuildMaterial("G4_WATER");
  //material ============================================
  //G4Element *Al = new G4Element("Aluminium", "Al", 13., 26.982 * g / mole);
  G4Element *Si = new G4Element("Silicon", "Si", 14., 28.086 * g / mole);
  G4Element *Cr = new G4Element("Chromium", "Cr", 24., 51.996 * g / mole);
  G4Element *Mn = new G4Element("Manganese", "Mn", 25., 54.938 * g / mole);
  G4Element *Fe = new G4Element("Iron", "Fe", 26., 55.85 * g / mole);
  G4Element *Ni = new G4Element("Nickel", "Ni", 28., 58.693 * g / mole);
  //==== EJ-276 for Scintillator target in BeamPipe ====

  //G4double EJ276_density = 1.099 * g / cm3;
  G4double H_density = 4.647e+22*(1. / cm3);
  G4double C_density = 4.944e+22*(1. / cm3);

  G4double H_M = nist->FindOrBuildElement("H")->GetAtomicMassAmu()*(g / mole);
  G4double C_M = nist->FindOrBuildElement("C")->GetAtomicMassAmu()*(g / mole);

  G4double H_MassDensity  = (H_density/CLHEP::Avogadro)*H_M;
  G4double C_MassDensity    = (C_density/CLHEP::Avogadro)*C_M;

  G4double HC_density = H_MassDensity + C_MassDensity;
  G4double H_Frac = H_MassDensity/HC_density;
  G4double C_Frac 	= C_MassDensity/HC_density;

  EJ276 = new G4Material("EJ276", HC_density, 2, kStateSolid);
  EJ276->AddElement(nist->FindOrBuildElement("H"), H_Frac);
  EJ276->AddElement(nist->FindOrBuildElement("C"), C_Frac);

  G4cout << "EJ276 : density " <<  EJ276->GetDensity()/(g / cm3) << " , "
         << "NbOfAtomsPerVolume " << EJ276->GetTotNbOfAtomsPerVolume()/(1. / cm3) << G4endl;
  //==== Stainless Steel ====
  SS304LSteel =
    new G4Material("SS304LSteel", 8.00 * g / cm3, 5, kStateSolid);
  SS304LSteel->AddElement(Fe, 0.65);
  SS304LSteel->AddElement(Cr, 0.20);
  SS304LSteel->AddElement(Ni, 0.12);
  SS304LSteel->AddElement(Mn, 0.02);
  SS304LSteel->AddElement(Si, 0.01);
  //==========================================================================================
}
G4VPhysicalVolume* DetectorConstruction::Construct()
{  
  
  // Env is water tank
  //
  G4double env_sizeXY = 3*m, env_sizeZ = 5*m;
  // Option to switch on/off checking of volumes overlaps
  //
  G4bool checkOverlaps = true;
 
  // World
  G4double world_sizeXY = 1.2*env_sizeXY;
  G4double world_sizeZ  = 1.2*env_sizeZ;
  
  
  G4Box* solidWorld =    
    new G4Box("World",                       //its name
       0.5*world_sizeXY, 0.5*world_sizeXY, 0.5*world_sizeZ);     //its size
      
  G4LogicalVolume* logicWorld =                         
    new G4LogicalVolume(solidWorld,          //its solid
                        Air,           //its material
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
  // Envelope
  //  
  G4Box* solidEnv =    
    new G4Box("Envelope",                    //its name
        0.5*env_sizeXY, 0.5*env_sizeXY, 0.5*env_sizeZ); //its size
      
  G4LogicalVolume* logicEnv =                         
    new G4LogicalVolume(solidEnv,            //its solid
                        Water,             //its material
                        "Envelope");         //its name
               
  new G4PVPlacement(0,                       //no rotation
                    G4ThreeVector(),         //at (0,0,0)
                    logicEnv,                //its logical volume
                    "Envelope",              //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking  
  // Container=====================================================================
  G4ThreeVector pos1 = G4ThreeVector(0, 0, 0);
  G4double ContainerSize = 50*cm;
  G4double ScintillatorSize = 20*cm;

  G4Box* Container =    
    new G4Box("Container",                       //its name
       0.5*ContainerSize, 0.5*ContainerSize, 0.5*ContainerSize);     //its size
                      
  /*G4LogicalVolume* logicContainer =                         
    new G4LogicalVolume(Container,         //its solid
                        Air,          //its material
                        "Container");           //its name
         
  new G4PVPlacement(0,                       //no rotation
                    pos1,                    //at position
                    logicContainer,             //its logical volume
                    "Container",                //its name
                    logicEnv,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking*/  
  //GuidePipe===============================================================
  G4double GuidePipeTubHalfLength = 0.5*m;
  G4double GuidePipeTubOutRaius = 5*cm;
  G4double GuidePipeTubInnerRaius = 4.5*cm;
  G4ThreeVector pos2 = G4ThreeVector(0,GuidePipeTubHalfLength*sin(pi/4) + 0.25*ContainerSize,GuidePipeTubHalfLength*sin(pi/4));
  G4Tubs *GuidePipeTub =
    new G4Tubs("GuidePipeTub", 0. * cm, GuidePipeTubOutRaius,
               GuidePipeTubHalfLength, 0. * deg, 360. * deg);
  G4Tubs *AirGuidePipeTub =
    new G4Tubs("AirGuidePipeTub", 0. * cm, GuidePipeTubInnerRaius,
               GuidePipeTubHalfLength, 0. * deg, 360. * deg);
  G4VSolid *SteelGuidePipeTub =
    new G4SubtractionSolid("SteelBPipePlate", GuidePipeTub, AirGuidePipeTub,
                           0, G4ThreeVector());
  G4LogicalVolume* logicSteelGuidePipeTub =                         
    new G4LogicalVolume(SteelGuidePipeTub,         //its solid
                        SS304LSteel,          //its material
                        "SteelGuidePipeTub");           //its name
  G4RotationMatrix *GuidePipeRot = new G4RotationMatrix;
  GuidePipeRot->rotateX(45.*deg);
  new G4PVPlacement(GuidePipeRot,            //rotate 45 degree on X
                    pos2,                    //at position
                    logicSteelGuidePipeTub,             //its logical volume
                    "SteelGuidePipeTub",                //its name
                    logicEnv,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);
  /*G4LogicalVolume* logicAirGuidePipeTub =                         
    new G4LogicalVolume(AirGuidePipeTub,         //its solid
                        Air,          //its material
                        "AirGuidePipeTub");           //its name
              
  new G4PVPlacement(0,                       //no rotation relative to steel tub
                    G4ThreeVector(0,0,0),              //at center of steel tub
                    logicAirGuidePipeTub,             //its logical volume
                    "AirGuidePipeTub",                //its name
                    logicSteelGuidePipeTub,                //its mother volume (Steel tub)
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);*/
  //========================================================================

  //BeamPipe================================================================
  G4double BeamPipeTubHalfLength = 1*m;
  G4double BeamPipeTubOutRaius = 5*cm;
  G4double BeamPipeTubInnerRaius = 4.5*cm;
  G4ThreeVector pos3 = G4ThreeVector(0,0,BeamPipeTubHalfLength + 0.6*ScintillatorSize);
  G4Tubs *BeamPipeTub =
    new G4Tubs("BeamPipeTub", 0. * cm, BeamPipeTubOutRaius,
               BeamPipeTubHalfLength, 0. * deg, 360. * deg);
  G4Tubs *AirBeamPipeTub =
    new G4Tubs("AirBeamPipeTub", 0. * cm, BeamPipeTubInnerRaius,
               BeamPipeTubHalfLength, 0. * deg, 360. * deg);
  G4Tubs *DetectorTub =
    new G4Tubs("DetectorTub", 0. * cm, BeamPipeTubInnerRaius,
               5*cm, 0. * deg, 360. * deg);
  G4VSolid *SteelBeamPipeTub =
    new G4SubtractionSolid("SteelBeamPipeTub", BeamPipeTub, AirBeamPipeTub,
                           0, G4ThreeVector());
  G4LogicalVolume* logicSteelBeamPipeTub =                         
    new G4LogicalVolume(SteelBeamPipeTub,         //its solid
                        SS304LSteel,          //its material
                        "SteelBeamPipeTub");           //its name
  //G4RotationMatrix *BeamPipeRot = new G4RotationMatrix;
  //BeamPipeRot->rotateX(90.*deg);
  new G4PVPlacement(0,            
                    pos3,                    //at 0 position
                    logicSteelBeamPipeTub,             //its logical volume
                    "SteelBeamPipeTub",                //its name
                    logicEnv,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);
  G4LogicalVolume* logicDetectorTub =                         
    new G4LogicalVolume(DetectorTub,         //its solid
                        Water,          //its material
                        "DetectorTub");           //its name
  //========================================================================
  //Whole air space=========================================================
  auto AirTee =
    new G4UnionSolid("AirTee", Container, AirBeamPipeTub, 
                     0,pos3);
  AirTee =
    new G4UnionSolid("AirTee", AirTee, AirGuidePipeTub, 
                     GuidePipeRot,pos2);
  G4LogicalVolume* logicAirTee =                         
    new G4LogicalVolume(AirTee,         //its solid
                        Air,          //its material
                        "AirTee");           //its name
  new G4PVPlacement(0,                       //no rotation
                    pos1,                    //at position
                    logicAirTee,             //its logical volume
                    "AirTee",                //its name
                    logicEnv,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking
  G4VisAttributes* DetectorVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,.0,0.3));  //set translucent
  logicAirTee->SetVisAttributes(DetectorVisAtt);

  new G4PVPlacement(0,            
                    G4ThreeVector(0,0,BeamPipeTubHalfLength),        //at center of BeamPipe
                    logicDetectorTub,             //its logical volume
                    "DetectorTub",                //its name
                    logicAirTee,      //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);
  //========================================================================
  //scintillator===============================================================     
  G4Box* Scintillator =    
    new G4Box("Scintillator",                       //its name
       0.5*ScintillatorSize, 0.5*ScintillatorSize, 0.5*ScintillatorSize);     //its size
                      
  G4LogicalVolume* logicScintillator =                         
    new G4LogicalVolume(Scintillator,         //its solid
                        Air,          //its material
                        "Scintillator");           //its name
               
  new G4PVPlacement(0,                       //no rotation
                    G4ThreeVector(0,0,-5*cm),       //at position
                    logicScintillator,             //its logical volume
                    "Scintillator",                //its name
                    logicAirTee,                //its mother volume  is contanier
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking
  //Set color of scintillator
  G4Colour GetterMainCylinderColor(G4Colour::Red());
  G4VisAttributes *ScintillatorVisAtt =
    new G4VisAttributes(GetterMainCylinderColor);
  ScintillatorVisAtt->SetForceAuxEdgeVisible(true);
  logicScintillator->SetVisAttributes(
    ScintillatorVisAtt);
  //===============================================================
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
