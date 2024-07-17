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
  G4Element *H = new G4Element("Hydrogen", "H", 1., 1.0079 * g / mole);
  G4Element *C = new G4Element("Carbon", "C", 6., 12.011 * g / mole);
  G4Element *O = new G4Element("Oxygen", "O", 8., 15.999 * g / mole);
  G4Element *D = new G4Element("Deuterium", "D", 1);
  G4Isotope *D_Iso = new G4Isotope("D_Iso", 1, 2, 2.014102 * g / mole);
  D->AddIsotope(D_Iso, 1);
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
  
  //==== EJ-315 for Scintillator target in BeamPipe ====
  // https://eljentechnology.com/products/liquid-scintillators/ej-315

  C6H6 = new G4Material("C6H6", 0.877 * g / cm3, 2, kStateLiquid);
  C6H6->AddElement(C, 6);
  C6H6->AddElement(H, 6);

  C6D6 = new G4Material("C6D6", 0.950 * g / cm3, 2, kStateLiquid);
  C6D6->AddElement(C, 6);
  C6D6->AddElement(D, 6);

  EJ315 =
    new G4Material("EJ315", 0.863 * g / cm3, 2, kStateLiquid);
  G4double fraction_d = 141.0/142.0;
  G4double fraction_h = 1.0/142.0;
  EJ315->AddMaterial(C6D6, fraction_d);
  EJ315->AddMaterial(C6H6, fraction_h);

  G4cout << "EJ315 : density " <<  EJ315->GetDensity()/(g / cm3) << " , "
         << "NbOfAtomsPerVolume " << EJ315->GetTotNbOfAtomsPerVolume()/(1. / cm3) << G4endl;
  //==== Stainless Steel ====
  SS304LSteel = new G4Material("SS304LSteel", 8.00 * g / cm3, 5, kStateSolid);
  SS304LSteel->AddElement(Fe, 0.65);
  SS304LSteel->AddElement(Cr, 0.20);
  SS304LSteel->AddElement(Ni, 0.12);
  SS304LSteel->AddElement(Mn, 0.02);
  SS304LSteel->AddElement(Si, 0.01);
  //====  Heavy Water ===
  HeavyWater = new G4Material("HeavyWater", 1.11*g/cm3, 2,
                                   kStateLiquid, 293.15*kelvin, 1*atmosphere);
  HeavyWater->AddElement(D, 2);
  HeavyWater->AddElement(O, 1);
  //===== Fefliter ======
  Fefliter = new G4Material("Fefliter", 7.86 * g / cm3, 1, kStateSolid);
  Fefliter->AddElement(Fe, 1.0);
  //===== C6D8 =====
  C6D8 = new G4Material("Hexane", 0.767*g/cm3, 2,
                                   kStateLiquid, 298.15*kelvin, 1*atmosphere);
  C6D8->AddElement(D, 2);
  C6D8->AddElement(O, 1);
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
                        Air,             //its material
                        "Envelope");         //its name
               
  new G4PVPlacement(0,                       //no rotation
                    G4ThreeVector(),         //at (0,0,0)
                    logicEnv,                //its logical volume
                    "Envelope",              //its name
                    logicWorld,              //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking  
  // Water tank=====================================================================
  //
  G4ThreeVector pos1 = G4ThreeVector(0, 0, -35*cm);  // Watertank
  G4ThreeVector pos2 = G4ThreeVector(0,0,-33.5*cm);  // AirTub
  G4double WaterTankLength = 380*cm;
  G4double LongPipeTubOutRaius = 2.6*cm;
  G4double LongPipeTubHalfLength = 0.5*3.77*m;

  G4Box* Container =    
    new G4Box("WaterContainer",                       //its name
       0.5*env_sizeXY - 1*cm, 0.5*env_sizeXY - 1*cm, 0.5*WaterTankLength);
  G4Tubs *LongTub =
    new G4Tubs("LongCutTub", 0, LongPipeTubOutRaius,
               LongPipeTubHalfLength, 0. * deg, 360. * deg);
  G4VSolid *ContainerCut =
    new G4SubtractionSolid("WaterContainer", Container, LongTub,
                           0, pos2-pos1);
  G4LogicalVolume* logicWaterTank =                         
    new G4LogicalVolume(ContainerCut,         //its solid
                        Water,          //its material
                        "logicWaterTank"); 
  new G4PVPlacement(0,            //rotate 45 degree on X
                    pos1,                    //at position
                    logicWaterTank,             //its logical volume
                    "WaterTank",                //its name
                    logicEnv,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);

  //LongPipe&Fefliter==================================================
  
  G4double FePipeTubHalfLength = 1*cm;
  
  G4double LongPipeTubInnerRaius = 2.5*cm;
  
  G4Tubs *LongPipeTub =
    new G4Tubs("LongPipeTub", LongPipeTubInnerRaius, LongPipeTubOutRaius,
               LongPipeTubHalfLength, 0. * deg, 360. * deg);
  G4Tubs *AirLongPipeTub =
    new G4Tubs("AirLongPipeTub", 0. * cm, LongPipeTubInnerRaius,
               LongPipeTubHalfLength, 0. * deg, 360. * deg);
  G4Tubs *SteelLongPipeTub =
    new G4Tubs("SteelLongPipeTub", LongPipeTubInnerRaius, LongPipeTubOutRaius,
               LongPipeTubHalfLength, 0. * deg, 360. * deg);
  //G4VSolid *SteelLongPipeTub =
    //new G4SubtractionSolid("SteelBPipePlate", LongPipeTub, AirLongPipeTub,
      //                     0, G4ThreeVector());
  G4Tubs *FefliterTub =
    new G4Tubs("FefliterTub", 0. * cm, LongPipeTubInnerRaius,
               FePipeTubHalfLength, 0. * deg, 360. * deg);                  
  G4LogicalVolume* logicFefliterTub =                         
    new G4LogicalVolume(FefliterTub,         //its solid
                        Fefliter,          //its material
                        "FefliterTub");
  G4LogicalVolume* logicSteelLongPipeTub =                         
    new G4LogicalVolume(SteelLongPipeTub,         //its solid
                        SS304LSteel,          //its material
                        "SteelLongPipeTub");           //its name
  //G4RotationMatrix *LongPipeRot = new G4RotationMatrix;
  //GuidePipeRot->rotateX(45.*deg);
  new G4PVPlacement(0,            //rotate 0
                    pos2,                    //at position
                    logicSteelLongPipeTub,             //its logical volume
                    "SteelLongPipeTub",                //its name
                    logicEnv,                //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);
  G4LogicalVolume* logicAirLongPipeTub =                         
    new G4LogicalVolume(AirLongPipeTub,         //its solid
                        Air,          //its material
                        "AirLongPipeTub");           //its name
              
  new G4PVPlacement(0,                       //no rotation relative to steel tub
                    pos2,              //at center of steel tub
                    logicAirLongPipeTub,             //its logical volume
                    "AirLongPipeTub",                //its name
                    logicEnv,                //its mother volume (Steel tub)
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);
  new G4PVPlacement(0,            
                    G4ThreeVector(0,0,0*cm),        //Behind DetectorTub
                    logicFefliterTub,             //its logical volume
                    "FefliterTub",                //its name
                    logicAirLongPipeTub,      //its mother  volume
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);
  //========================================================================
  //Reflector===============================================================  
  G4double ReflectorLength = 19*cm;   
  G4Tubs* Reflector =    
    new G4Tubs("Reflector",                       //its name
       0 * cm, LongPipeTubInnerRaius, 0.5*ReflectorLength,0. * deg, 360. * deg);     //its size
  G4Tubs* ReflectorContainer =    
    new G4Tubs("ReflectorContainer",                       //its name
       0 * cm, LongPipeTubOutRaius, 0.5*ReflectorLength+0.5*cm,0. * deg, 360. * deg);                    
  G4LogicalVolume* logicReflector =                         
    new G4LogicalVolume(Reflector,         //its solid
                        HeavyWater,          //its material
                        "Reflector");           //its name
  G4LogicalVolume* logicReflectorContainer =                         
    new G4LogicalVolume(ReflectorContainer,         //its solid
                        SS304LSteel,          //its material
                        "ReflectorContainer");  
  new G4PVPlacement(0,                       //no rotation
                    G4ThreeVector(0,0,225*cm-0.5*ReflectorLength-0.5*cm),       //at position
                    logicReflectorContainer,             //its logical volume
                    "ReflectorContainer",                //its name
                    logicEnv,                //its mother volume  is contanier
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);            
  new G4PVPlacement(0,                       //no rotation
                    G4ThreeVector(0,0,0*cm),       //at position
                    logicReflector,             //its logical volume
                    "Scintillator",                //its name
                    logicReflectorContainer,                //its mother volume  is contanier
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking
  //Set color of scintillator
  G4Colour GetterMainCylinderColor(G4Colour::Red());
  G4VisAttributes *ScintillatorVisAtt =
    new G4VisAttributes(GetterMainCylinderColor);
  ScintillatorVisAtt->SetForceAuxEdgeVisible(true);
  logicReflectorContainer->SetVisAttributes(
    ScintillatorVisAtt);
  //===============================================================
  //Detector===============================================================  
  G4double DetectorLength = 3*cm;   
  G4Tubs* Detector =    
    new G4Tubs("Detector",                       //its name
       0 * cm, LongPipeTubInnerRaius, 0.5*DetectorLength,0. * deg, 360. * deg);
  G4LogicalVolume* logicDetector =                         
    new G4LogicalVolume(Detector,         //its solid
                        Air,          //its material
                        "Detector");           //its name
  new G4PVPlacement(0,                       //no rotation
                    G4ThreeVector(0,0,-0.5*(WaterTankLength - DetectorLength)),   //at position
                    logicDetector,             //its logical volume
                    "Detector",                //its name
                    logicWaterTank,                //its mother volume  is contanier
                    false,                   //no boolean operation
                    0,                       //copy number
                    checkOverlaps);          //overlaps checking
  return physWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
