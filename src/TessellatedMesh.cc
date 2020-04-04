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

// CADMesh //
#include "TessellatedMesh.hh"

// GEANT4 //
#include "G4UIcommand.hh"
#include "Randomize.hh"


namespace CADMesh
{

G4VSolid* TessellatedMesh::GetSolid()
{
    return (G4VSolid*) GetTessellatedSolid();
}


G4VSolid* TessellatedMesh::GetSolid(G4int index)
{
    return (G4VSolid*) GetTessellatedSolid(index);
}


G4VSolid* TessellatedMesh::GetSolid(G4String name)
{
    return (G4VSolid*) GetTessellatedSolid(name);
}


G4AssemblyVolume* TessellatedMesh::GetAssembly()
{
    if (assembly_)
    {
        return assembly_;
    }

    for (auto mesh : reader_->GetMeshes())
    {
        auto solid = GetTessellatedSolid(mesh); 

        // TODO: Determine the material for this solid.
        G4Material* material = nullptr;

        auto logical = new G4LogicalVolume( solid
                                          , material 
                                          , mesh->GetName() + "_logical"
        );

        G4ThreeVector position = G4ThreeVector();
        G4RotationMatrix* rotation = new G4RotationMatrix();

        assembly_->AddPlacedVolume(logical, position, rotation);
    }

    return assembly_;    
}


G4TessellatedSolid* TessellatedMesh::GetTessellatedSolid()
{
    return GetTessellatedSolid(0);
}


G4TessellatedSolid* TessellatedMesh::GetTessellatedSolid(G4int index)
{
    auto mesh = reader_->GetMesh(index);

    return GetTessellatedSolid(mesh);
}


G4TessellatedSolid* TessellatedMesh::GetTessellatedSolid(G4String /*name*/)
{
    // TODO: Get the tesselated solid by name.

    return nullptr;
}


G4TessellatedSolid* TessellatedMesh::GetTessellatedSolid(
        std::shared_ptr<Mesh> mesh)
{
    auto volume_solid = new G4TessellatedSolid(mesh->GetName());

    for(auto triangle : mesh->GetTriangles())
    {
        auto a = triangle->GetVertex(0) * scale_ + offset_;
        auto b = triangle->GetVertex(1) * scale_ + offset_;
        auto c = triangle->GetVertex(2) * scale_ + offset_;
       
        auto t = new G4TriangularFacet(a, b, c, ABSOLUTE);

        if (reverse_)
        {
            volume_solid->AddFacet((G4VFacet*) t->GetFlippedFacet());
        }
       
        else
        {
            volume_solid->AddFacet((G4VFacet*) t);
        }
    }

    volume_solid->SetSolidClosed(true);
    /*
    if (volume_solid->GetNumberOfFacets() == 0) {
        G4Exception( "TessellatedMesh::GetTessellatedSolid", "The loaded mesh has 0 faces."
                   , FatalException, "The file may be empty.");
        return nullptr;
    }
    */
    return volume_solid;
}

} // CADMesh namespace

