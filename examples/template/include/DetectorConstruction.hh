/* ************************************************
 * GEANT4 CAD INTERFACE - template
 *
 * File:      DetectorConstruction.hh
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Date:      17th August, 2017
 **************************************************/

#pragma once

// STL //
#include <string>

// GEANT4 //
class G4VSolid;
class G4LogicalVolume;
class G4VPhysicalVolume;

#include "G4ThreeVector.hh"
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
    
    G4VSolid * cad_solid;
    G4LogicalVolume * cad_logical;
    G4VPhysicalVolume * cad_physical;
};

