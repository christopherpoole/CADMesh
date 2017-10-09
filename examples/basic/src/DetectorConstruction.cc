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
#include "G4Transform3D.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4AssemblyVolume.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"


DetectorConstruction::DetectorConstruction()
{
    filename = "";
    filetype = "";
}

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
    world_logical->SetVisAttributes(G4VisAttributes::Invisible);

    // CAD model rotation. 
    G4RotationMatrix * rot = new G4RotationMatrix();
    rot->rotateZ(90*deg);
    
    // Load CAD file as tessellated solid //
    offset = G4ThreeVector(-20*cm, 0, 0);
    
    // Note that offset is applied to the points in mesh directly before placement.
    auto mesh = CADMesh::TessellatedMesh::FromPLY(filename);
    mesh->SetScale(mm);
    mesh->SetOffset(offset);
    mesh->SetReverse(false);

    cad_solid = mesh->GetSolid();
    cad_logical = new G4LogicalVolume(cad_solid, water, "cad_logical", 0, 0, 0);
    cad_physical = new G4PVPlacement(rot, G4ThreeVector(), cad_logical,
                                     "cad_physical", world_logical, false, 0);
    cad_logical->SetVisAttributes(G4Color(0.5, 0.3, 1, 1));
   
    
    // Load CAD file as tetrahedral mesh //
    auto tet_mesh = CADMesh::TetrahedralMesh::FromPLY(filename);
    tet_mesh->SetScale(1.5);
    tet_mesh->SetMaterial(water); // We have to do this before making the G4AssemblyVolume.

    G4AssemblyVolume * cad_assembly = tet_mesh->GetAssembly();

    G4Translate3D translation(20*cm, 0., 0.);
    G4Transform3D rotation = G4Rotate3D(*rot);
    G4Transform3D transform = translation*rotation;

    cad_assembly->MakeImprint(world_logical, transform, 0, 0);

    return world_physical;
}

