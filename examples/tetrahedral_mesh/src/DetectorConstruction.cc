#include <G4VPhysicalVolume.hh>
#include <G4NistManager.hh>
#include <G4Material.hh>
#include <G4Box.hh>
#include <G4SystemOfUnits.hh>
#include <G4LogicalVolume.hh>
#include <G4VisAttributes.hh>
#include <G4PVPlacement.hh>
#include <G4ThreeVector.hh>
#include <G4Colour.hh>
#include <G4RotationMatrix.hh>

#include "cadmesh.hh"

#include "DetectorConstruction.hh"


DetectorConstruction::DetectorConstruction()
  : fWorldSolid(nullptr), fWorldLogical(nullptr), fWorldPhysical(nullptr),
    fTetAssembly(), fPath("")
{
}

DetectorConstruction::~DetectorConstruction()
{
  // deletion of member data through Geant4's object stores
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  G4NistManager* nistManager = G4NistManager::Instance();
  G4Material* air = nistManager->FindOrBuildMaterial("G4_AIR");
  G4Material* adiposeTissue = nistManager->FindOrBuildMaterial("G4_ADIPOSE_TISSUE_ICRP");
  G4Material* heartMuscle = nistManager->FindOrBuildMaterial("G4_MUSCLE_STRIATED_ICRU");

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

  // compare with the region attributes defined in the .ele file
  constexpr G4int SKIN_ID = 2;
  constexpr G4int HEART_ID = 1;

  // region attributes
  cadmesh::RegionAttributeMap regionAttributeMap;
  regionAttributeMap[SKIN_ID] = {G4Colour::White(), adiposeTissue};
  regionAttributeMap[HEART_ID] = {G4Colour::Red(), heartMuscle};

  // set up file reader instance
  cadmesh::TetrahedralMeshFileReader fileReader;
  fileReader.SetUnitOfLength(mm);

  // read assembly of tetrahedra
  fTetAssembly = fileReader.ReadTetAssemblyVolume(fPath, regionAttributeMap);

  //**************************************************************************************

  // translation/rotation of whole tet assembly
  // e.g.:
  G4RotationMatrix* rot = new G4RotationMatrix();
  rot->rotateX(90*deg);
  G4ThreeVector nullVector = G4ThreeVector();

  // place all tetrahedra in world
  fTetAssembly->MakeImprint(fWorldLogical,   // mother logical
                            nullVector,      // translation w.r.t. mother
                            rot,             // rotation w.r.t. mother
                            false);          // check surfaces for overlap

  return fWorldPhysical;
}
