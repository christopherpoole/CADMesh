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

TessellatedMesh::TessellatedMesh(const aiScene* scene)
        : CADMeshTemplate<TessellatedMesh>(), scene_(scene)
{
}


TessellatedMesh::TessellatedMesh(G4String file_name, File::Type file_type)
        : CADMeshTemplate<TessellatedMesh>(file_name, file_type)
{
    if (scene_)
    {
        return;
    }

    importer_ = new Assimp::Importer();


    scene_ = importer_->ReadFile(file_name,
            aiProcess_Triangulate           |
            aiProcess_JoinIdenticalVertices |
            aiProcess_CalcTangentSpace);

    if (!scene_)
    {
        G4Exception( "TessellatedMesh::GetScene", "The mesh cannot be loaded."
                   , FatalException, "The file may not exist.");
    }
}


TessellatedMesh::~TessellatedMesh()
{
}


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

    for (unsigned int index = 0; index < scene_->mNumMeshes; index++)
    {
        G4String name = file_name_
                      + G4String("_mesh_")
                      + G4UIcommand::ConvertToString((int) index);

        auto solid = GetSolid(index); 

        // TODO: Determine the material for this solid.
        G4Material* material = nullptr;

        auto logical = new G4LogicalVolume( solid
                                          , material 
                                          , name
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
    auto mesh = GetMesh(index);

    auto volume_solid = new G4TessellatedSolid(file_name_);

    G4ThreeVector point_1;
    G4ThreeVector point_2;
    G4ThreeVector point_3;

    for(unsigned int i=0; i < mesh->mNumFaces; i++)
    {
        const aiFace& face = mesh->mFaces[i];

        point_1.setX(mesh->mVertices[face.mIndices[0]].x * scale_ + offset_.x());
        point_1.setY(mesh->mVertices[face.mIndices[0]].y * scale_ + offset_.y());
        point_1.setZ(mesh->mVertices[face.mIndices[0]].z * scale_ + offset_.z());

        point_2.setX(mesh->mVertices[face.mIndices[1]].x * scale_ + offset_.x());
        point_2.setY(mesh->mVertices[face.mIndices[1]].y * scale_ + offset_.y());
        point_2.setZ(mesh->mVertices[face.mIndices[1]].z * scale_ + offset_.z());

        point_3.setX(mesh->mVertices[face.mIndices[2]].x * scale_ + offset_.x());
        point_3.setY(mesh->mVertices[face.mIndices[2]].y * scale_ + offset_.y());
        point_3.setZ(mesh->mVertices[face.mIndices[2]].z * scale_ + offset_.z());
        
        G4TriangularFacet * facet;
        if (reverse_ == false) {
            facet = new G4TriangularFacet(point_1, point_2, point_3, ABSOLUTE);
        } else {
            facet = new G4TriangularFacet(point_2, point_1, point_3, ABSOLUTE);
        }
        volume_solid->AddFacet((G4VFacet*) facet);
    }

    volume_solid->SetSolidClosed(true);

    if (volume_solid->GetNumberOfFacets() == 0) {
        G4Exception( "TessellatedMesh::GetTessellatedSolid", "The loaded mesh has 0 faces."
                   , FatalException, "The file may be empty.");
        return 0;
    }

    return volume_solid;
}


G4TessellatedSolid* TessellatedMesh::GetTessellatedSolid(G4String name)
{
    for (unsigned int index = 0; index < scene_->mNumMeshes; index++) {
        aiMesh* mesh = scene_->mMeshes[index];

        if (strcmp(mesh->mName.C_Str(), name.c_str()))
            return GetTessellatedSolid(index);
    }

    G4Exception( "TessellatedMesh::GetTessellatedSolid", "The mesh could not be found."
               , FatalException, name);


    return nullptr;
}  


aiMesh* TessellatedMesh::GetMesh()
{
    return GetMesh(0);
}


aiMesh* TessellatedMesh::GetMesh(unsigned int index)
{
    if (index > scene_->mNumMeshes)
    {
        G4Exception( "TessellatedMesh::GetMesh", "Index out of bounds."
                   , FatalException, "");

        return nullptr;
    }

    return scene_->mMeshes[index];
}


aiMesh* TessellatedMesh::GetMesh(G4String name)
{
    for (unsigned int index = 0; index < scene_->mNumMeshes; index++) {
        aiMesh* mesh = scene_->mMeshes[index];

        if (strcmp(mesh->mName.C_Str(), name.c_str()))
        {
            return mesh;
        }
    }
    
    G4Exception( "TessellatedMesh::GetMesh", "The mesh could not be found."
               , FatalException, "");

    return nullptr;
}

    
bool TessellatedMesh::IsValidForNavigation()
{
    return IsValidForNavigation(0);
}


bool TessellatedMesh::IsValidForNavigation(G4String name)
{
    return IsValidForNavigation(GetMesh(name));
}


bool TessellatedMesh::IsValidForNavigation(G4int index)
{
    return IsValidForNavigation(GetMesh(index));
}


bool TessellatedMesh::IsValidForNavigation(aiMesh* mesh)
{
    typedef std::pair<int, int> Edge; // such that Edge.first < Edge.second

    std::map<Edge, int> edge_use_count;


    for(unsigned int i=0; i < mesh->mNumFaces; i++)
    {
        const aiFace& face = mesh->mFaces[i];

        int a = face.mIndices[0];
        int b = face.mIndices[1];
        int c = face.mIndices[2];

        if (a < b)
        {
            edge_use_count[Edge(a, b)] += 1; 
        }

        else if (a > b)
        {
            edge_use_count[Edge(b, a)] += 1; 
        }

        if (b < c)
        {
            edge_use_count[Edge(b, c)] += 1; 
        }

        else if (b > c)
        {
            edge_use_count[Edge(c, b)] += 1; 
        }

        if (c < a)
        {
            edge_use_count[Edge(c, a)] += 1; 
        }

        else if (c > a)
        {
            edge_use_count[Edge(a, c)] += 1; 
 
        }
    }

    // Every edge should be shared with exactly two triangles.
    for (auto count : edge_use_count)
    {
        if (count.second != 2)
        {
            return false;
        }
    }
   
    return true; 
}

}

