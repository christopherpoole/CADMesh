#include <G4VPhysicalVolume.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4Box.hh>
#include <G4SystemOfUnits.hh>
#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>
#include <G4PVPlacement.hh>
#include <G4ThreeVector.hh>
#include <G4TessellatedSolid.hh>
#include <G4Colour.hh>
#include <G4RotationMatrix.hh>

#include "cadmesh.hh"

#include "DetectorConstruction.hh"


DetectorConstruction::DetectorConstruction()
  : fWorldSolid(nullptr), fWorldLogical(nullptr), fWorldPhysical(nullptr),
    fPolygonMeshSolid(nullptr), fPolygonMeshLogical(nullptr),
    fPolygonMeshPhysical(nullptr), fPath("")
{
}

DetectorConstruction::~DetectorConstruction()
{
  // deletion of data member through Geant4's object stores
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  G4NistManager* nistManager = G4NistManager::Instance();
  G4Material* air = nistManager->FindOrBuildMaterial("G4_AIR");
  G4Material* water = nistManager->FindOrBuildMaterial("G4_WATER");

  fWorldSolid = new G4Box("world_solid", 50*cm, 50*cm, 50*cm);
  fWorldLogical = new G4LogicalVolume(fWorldSolid, air, "world_logical");
  fWorldLogical->SetVisAttributes(G4VisAttributes::Invisible);

  fWorldPhysical = new G4PVPlacement(nullptr,           // rotation
                                     G4ThreeVector(),   // translation
                                     fWorldLogical,     // current logical
                                     "world_physical",  // name 
                                     nullptr,           // mother volume
                                     false,
                                     0);                // copy number
  

  //**************************************************************************************
  //    CADMesh Part
  //**************************************************************************************

  // Set verbosity of CADMesh's log output
  cadmesh::SetVerbosityLevel(cadmesh::DEBUG);
  
  // Set up file reader instance
  cadmesh::PolygonMeshFileReader fileReader;
  fileReader.SetUnitOfLength(mm);
  fileReader.SetPermuteFacetPoints(false);

  // read single polygon mesh from file
  cadmesh::PolygonMesh polygonMesh = fileReader.ReadMesh(fPath);
  fPolygonMeshSolid = polygonMesh.tessellatedSolid;

  //**************************************************************************************

  fPolygonMeshLogical = new G4LogicalVolume(fPolygonMeshSolid, water, "cad_logical");
  fPolygonMeshLogical->SetVisAttributes(G4Colour::Blue());

  G4RotationMatrix* rot = new G4RotationMatrix();
  rot->rotateZ(90*deg);
  G4ThreeVector translation = G4ThreeVector(0, 0, 0);

  fPolygonMeshPhysical = new G4PVPlacement(rot,                 // rotation  
                                           translation,         // translation
                                           fPolygonMeshLogical, // current logical
                                           "cad_physical",      // name
                                           fWorldLogical,       // mother volume
                                           false,
                                           0);                  // copy number

  return fWorldPhysical;
}
