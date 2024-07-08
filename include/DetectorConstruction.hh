/// \file DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4NistManager.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

/// Detector construction class to define materials and geometry.

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    void DefineMaterial();
    //G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; }
    
  protected:
    G4LogicalVolume*  fScoringVolume;
    G4Material *Air,*Water,*EJ276,*EJ315,*SS304LSteel,*C6D8,*HeavyWater;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

