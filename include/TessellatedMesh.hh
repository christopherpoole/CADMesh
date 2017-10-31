/* ************************************************
 * GEANT4 CAD INTERFACE
 *
 * File:      TessellatedMesh.hh
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Original:  7th March, 2011
 **************************************************/

#pragma once

// CADMesh //
#include "CADMeshTemplate.hh"

// Open Asset Importer Library //
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

// GEANT4 //
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4TessellatedSolid.hh"
#include "G4TriangularFacet.hh"
#include "G4Tet.hh"
#include "G4AssemblyVolume.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"
#include "G4UIcommand.hh"


namespace CADMesh
{

class TessellatedMesh : public CADMeshTemplate<TessellatedMesh>
{
  using CADMeshTemplate::CADMeshTemplate;

  public:
    G4VSolid* GetSolid();
    G4VSolid* GetSolid(G4int index);
    G4VSolid* GetSolid(G4String name);

    G4TessellatedSolid* GetTessellatedSolid();
    G4TessellatedSolid* GetTessellatedSolid(G4int index);
    G4TessellatedSolid* GetTessellatedSolid(G4String name);
    G4TessellatedSolid* GetTessellatedSolid(std::shared_ptr<Mesh> mesh);

    G4AssemblyVolume* GetAssembly();

  public:
    void SetReverse(G4bool reverse) {
        this->reverse_ = reverse;
    };

    G4bool GetReverse() {
        return this->reverse_;
    };
    
  private:
    G4bool reverse_;
};

} // CADMesh namespace

