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
  public:
    TessellatedMesh()
        : CADMeshTemplate<TessellatedMesh>()
    {
    };

    TessellatedMesh(G4String file_name)
        : TessellatedMesh(file_name, File::Unknown)
    {
    };

    TessellatedMesh(G4String file_name, File::Type file_type);

    TessellatedMesh(const aiScene* scene);
    ~TessellatedMesh();

  public:
    G4TessellatedSolid* GetSolid();
    G4TessellatedSolid* GetSolid(G4int index);
    G4TessellatedSolid* GetSolid(G4String name);

    aiMesh* GetMesh();
    aiMesh* GetMesh(unsigned int index);
    aiMesh* GetMesh(G4String name);
    
    bool IsValidForNavigation();
    bool IsValidForNavigation(G4int index);
    bool IsValidForNavigation(G4String name);
        
  public:
    void SetReverse(G4bool reverse) {
        this->reverse_ = reverse;
    };

    G4bool GetReverse() {
        return this->reverse_;
    };
    
  private:
    Assimp::Importer* importer_;
    const aiScene* scene_;

    G4bool reverse_;
};

} // CADMesh namespace

