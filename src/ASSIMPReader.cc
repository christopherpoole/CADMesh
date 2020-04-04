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

#ifdef USE_CADMESH_ASSIMP_READER

#include "ASSIMPReader.hh"

namespace CADMesh
{

namespace File
{

ASSIMPReader::ASSIMPReader() : Reader("ASSIMPReader")
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


G4bool ASSIMPReader::CanRead(Type /*file_type*/)
{
    // TODO: Assuming ASSIMP can read anything in the list of file types.
    return true;
}


std::shared_ptr<ASSIMPReader> ASSIMP()
{
    return std::make_shared<ASSIMPReader>();
}

} // File namespace 

} // CADMesh namespace

#endif

