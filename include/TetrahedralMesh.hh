// The MIT License (MIT)
//
// Copyright (c) 2011-2020 Christopher M. Poole <mail@christopherpoole.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

#ifdef USE_CADMESH_TETGEN

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
//#include "G4SystemOfUnits.hh"


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

#endif

