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

// STL //
#include <memory>

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
    using CADMeshTemplate::CADMeshTemplate;
    
    TetrahedralMesh();
    ~TetrahedralMesh();
  
  public:
    G4VSolid* GetSolid();
    G4VSolid* GetSolid(G4int index);
    G4VSolid* GetSolid(G4String name);

    G4AssemblyVolume* GetAssembly();

  public:
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

    std::shared_ptr<tetgenio> GetTetgenInput() {
        return in_;
    };

    std::shared_ptr<tetgenio> GetTetgenOutput() {
        return out_;
    };

  private:
    // Private helper functions.
    G4ThreeVector GetTetPoint(G4int index_offset);
 
  private:
    std::shared_ptr<tetgenio> in_ = nullptr;
    std::shared_ptr<tetgenio> out_ = nullptr;

    G4double quality_;

    G4Material* material_;
};

} // CADMesh namespace

