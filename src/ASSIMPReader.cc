
#include "ASSIMPReader.hh"

namespace CADMesh
{

namespace Readers
{

ASSIMPReader::ASSIMPReader()
{
    importer_ = new Assimp::Importer();
}


ASSIMPReader::~ASSIMPReader()
{
    delete importer_;
}


G4bool ASSIMPReader::Read(G4String filepath)
{
    auto scene = importer_->ReadFile( filepath.c_str(),
                                      aiProcess_Triangulate
                                    | aiProcess_JoinIdenticalVertices 
                                    | aiProcess_CalcTangentSpace);

    if (!scene)
    {
        Exceptions::FileNotFound("ASSIMP::Read", filepath);
        return false;
    }

    for (unsigned int index = 0; index < scene->mNumMeshes; index++)
    {
        aiMesh* mesh = scene->mMeshes[index];
        auto name = mesh->mName.C_Str();

        Triangles triangles;

        for(unsigned int i=0; i < mesh->mNumFaces; i++)
        {
            const aiFace& face = mesh->mFaces[i];

            G4ThreeVector a( mesh->mVertices[face.mIndices[0]].x
                           , mesh->mVertices[face.mIndices[0]].y 
                           , mesh->mVertices[face.mIndices[0]].z); 

            G4ThreeVector b( mesh->mVertices[face.mIndices[1]].x
                           , mesh->mVertices[face.mIndices[1]].y 
                           , mesh->mVertices[face.mIndices[1]].z); 

            G4ThreeVector c( mesh->mVertices[face.mIndices[2]].x
                           , mesh->mVertices[face.mIndices[2]].y 
                           , mesh->mVertices[face.mIndices[2]].z); 

            triangles.push_back(new G4TriangularFacet(a, b, c, ABSOLUTE));
        }

        AddMesh(Mesh::New(triangles, name));
    }

    return true;
}


std::shared_ptr<ASSIMPReader> ASSIMP()
{
    return std::make_shared<ASSIMPReader>();
}

} // Readers namespace 

} // CADMesh namespace

