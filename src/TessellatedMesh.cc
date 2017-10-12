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


G4TessellatedSolid* TessellatedMesh::GetSolid()
{
    return GetSolid(0);
}


G4TessellatedSolid* TessellatedMesh::GetSolid(G4int index)
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
        G4Exception( "TessellatedMesh::GetSolid", "The loaded mesh has 0 faces."
                   , FatalException, "The file may be empty.");
        return 0;
    }

    return volume_solid;
}


G4TessellatedSolid* TessellatedMesh::GetSolid(G4String name)
{
    for (unsigned int index = 0; index < scene_->mNumMeshes; index++) {
        aiMesh* mesh = scene_->mMeshes[index];

        if (strcmp(mesh->mName.C_Str(), name.c_str()))
            return GetSolid(index);
    }

    G4Exception( "TessellatedMesh::GetSolid", "The mesh could not be found."
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

}

