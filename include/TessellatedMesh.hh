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

// CADMesh //
#include "CADMeshTemplate.hh"

// GEANT4 //
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4TessellatedSolid.hh"
#include "G4TriangularFacet.hh"
#include "G4Tet.hh"
#include "G4AssemblyVolume.hh"
#include "G4Material.hh"
#include "G4LogicalVolume.hh"
//#include "G4SystemOfUnits.hh"
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

