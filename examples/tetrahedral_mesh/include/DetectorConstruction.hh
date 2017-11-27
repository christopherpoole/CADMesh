#ifndef DETETCTOR_CONSTRUCTION_HH
#define DETETCTOR_CONSTRUCTION_HH

#include <memory>

#include <G4VUserDetectorConstruction.hh>
#include <G4String.hh>
#include <G4VSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>

#include "cadmesh.hh"


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
    
    // tetrahedral mesh
    std::unique_ptr<cadmesh::TetAssemblyVolume> fTetAssembly;

    G4String fPath;
};

#endif  // DETECTOR_CONSTRUCTION_HH
