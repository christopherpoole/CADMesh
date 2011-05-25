/* ************************************************
 * GEANT4 VCGLIB/CAD INTERFACE - basic example
 *
 * File:      DetectorConstruction.hh
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Date:      20th March, 2011
 **************************************************/


#ifndef DetectorConstruction_H
#define DetectorConstruction_H 1

// CADMESH //
#include "CADMesh.hh"

// GEANT4 //
class G4VSolid;
class G4LogicalVolume;
class G4VPhysicalVolume;

#include "G4VUserDetectorConstruction.hh"

class DetectorConstruction : public G4VUserDetectorConstruction
{
  public:

    DetectorConstruction();
    ~DetectorConstruction();

    G4VPhysicalVolume* Construct();

  private:
    G4VSolid * world_solid;
    G4LogicalVolume* world_logical;
    G4VPhysicalVolume* world_physical;
    
    G4ThreeVector offset;
    CADMesh mesh;
    G4VSolid * cad_solid;
    G4LogicalVolume * cad_logical;
    G4VPhysicalVolume * cad_physical;
};

#endif

