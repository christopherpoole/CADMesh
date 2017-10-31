/* ************************************************
 * GEANT4 CAD INTERFACE
 *
 * File:      TessellatedMesh.cc
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Date:      7th March, 2011
 **************************************************/

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
        if (reverse_)
        {
            volume_solid->AddFacet((G4VFacet*) triangle->GetFlippedFacet());
        }
       
        else
        {
            volume_solid->AddFacet((G4VFacet*) triangle);
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

