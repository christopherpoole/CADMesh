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

    void SetCADFilename(std::string name) {
        filename = name;
    };

    void SetCADFiletype(std::string type) {
        filetype = type;
    };


  private:
    G4VSolid * world_solid;
    G4LogicalVolume* world_logical;
    G4VPhysicalVolume* world_physical;
    
    G4ThreeVector offset;
    G4VSolid * cad_solid;
    G4LogicalVolume * cad_logical;
    G4VPhysicalVolume * cad_physical;

    std::string filename;
    std::string filetype;
};

#endif

