/* ************************************************
 * GEANT4 CAD INTERFACE
 *
 * File:      TetrahedralMesh.hh
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Date:      7th March, 2011
 **************************************************/

#pragma once

// CADMesh //
#include "CADMeshTemplate.hh"

// TETGEN //
#include "tetgen.h"

// GEANT4 //
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4Tet.hh"
#include "G4AssemblyVolume.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"
#include "G4SystemOfUnits.hh"


namespace CADMesh
{

class TetrahedralMesh : public CADMeshTemplate<TetrahedralMesh>
{
  public:
    TetrahedralMesh()
        : CADMeshTemplate<TetrahedralMesh>()
    {
    };

    TetrahedralMesh(char* file_name)
        : CADMeshTemplate<TetrahedralMesh>(file_name)
    {
    };

    TetrahedralMesh(char* file_name, char* file_type)
        : CADMeshTemplate<TetrahedralMesh>(file_name, file_type)
    {
    };

    ~TetrahedralMesh();
  
  public:
    G4AssemblyVolume* GetAssembly();

    void SetMaterial(G4Material* material) {
        this->material_ = material;
    };

    G4Material* GetMaterial() {
        return this->material_;
    };

    void SetQuality(G4double quality) {
        this->quality_ = quality;
    };

    G4double GetQuality() {
        return this->quality_;
    };

    tetgenio GetTetgenInput() {
        return in;
    };

    tetgenio GetTetgenOutput() {
        return out;
    };

  private:
    // Private helper functions.
    G4ThreeVector GetTetPoint(G4int index_offset);
 
  private:
    tetgenio in;
    tetgenio out;

    G4double quality_;
    G4Material* material_;
};

} // CADMesh namespace

