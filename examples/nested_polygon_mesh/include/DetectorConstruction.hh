#ifndef DETETCTOR_CONSTRUCTION_HH
#define DETETCTOR_CONSTRUCTION_HH

#include <G4VUserDetectorConstruction.hh>
#include <G4VPhysicalVolume.hh>
#include <G4String.hh>
#include <G4ThreeVector.hh>
#include <G4VSolid.hh>
#include <G4LogicalVolume.hh>


class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    DetectorConstruction();
    ~DetectorConstruction();

    G4VPhysicalVolume* Construct();

    void SetCADFilename(const G4String& path) { fPath = path; }

  private:
    G4VSolid* fWorldSolid;
    G4LogicalVolume* fWorldLogical;
    G4VPhysicalVolume* fWorldPhysical;

    // nested meshes hierarchy (embedded in top level logical volume)
    G4LogicalVolume* fTopLevelLogical;
    G4VPhysicalVolume* fTopLevelPhysical;

    G4String fPath;
};

#endif  // DETECTOR_CONSTRUCTION_HH
