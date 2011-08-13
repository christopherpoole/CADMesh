/* ************************************************
 * GEANT4 VCGLIB/CAD INTERFACE - basic example
 *
 * File:      DetectorConstruction.cc
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Date:      20th March, 2011
 **************************************************/

// USER //
#include "DetectorConstruction.hh"

// CADMESH //
#include "CADMesh.hh"

// GEANT4 //
#include "globals.hh"
#include "G4ThreeVector.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"

DetectorConstruction::DetectorConstruction()
{;}

DetectorConstruction::~DetectorConstruction()
{
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    G4NistManager * nist_manager = G4NistManager::Instance();
    G4Material * air = nist_manager->FindOrBuildMaterial("G4_AIR");
    G4Material * water = nist_manager->FindOrBuildMaterial("G4_WATER");

    world_solid = new G4Box("world_solid", 50*cm, 50*cm, 50*cm);
    world_logical = new G4LogicalVolume(world_solid, air,"world_logical",0,0,0);
    world_physical = new G4PVPlacement(0, G4ThreeVector(), world_logical,
                                       "world_physical", 0, false, 0);

    offset = G4ThreeVector(10*cm, 0, 0);
    CADMesh * mesh = new CADMesh("../../models/sphere_large.ply", "PLY", mm, offset, false);

    cad_solid = mesh->TessellatedMesh();
    cad_logical = new G4LogicalVolume(cad_solid, water, "cad_logical", 0, 0, 0);
    cad_physical = new G4PVPlacement(0, G4ThreeVector(), cad_logical,
                                     "cad_physical", world_logical, false, 0);

    return world_physical;
}

