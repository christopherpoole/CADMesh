#ifndef DETETCTOR_CONSTRUCTION_HH
#define DETETCTOR_CONSTRUCTION_HH

#include <G4VUserDetectorConstruction.hh>
#include <G4String.hh>
#include <G4VSolid.hh>
#include <G4LogicalVolume.hh>
#include <G4VPhysicalVolume.hh>
#include <G4TessellatedSolid.hh>


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
    
    G4TessellatedSolid* fPolygonMeshSolid;
    G4LogicalVolume* fPolygonMeshLogical;
    G4VPhysicalVolume* fPolygonMeshPhysical;

    G4String fPath;
};

#endif  // DETECTOR_CONSTRUCTION_HH
