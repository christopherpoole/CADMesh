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
#include "Reader.hh"


namespace CADMesh
{

namespace File
{

Reader::Reader(G4String reader_name) : name_(reader_name)
{
}


Reader::~Reader()
{
}


G4String Reader::GetName()
{
    return name_;
}


std::shared_ptr<Mesh> Reader::GetMesh()
{
    if (meshes_.size() > 0)
    {
        return meshes_[0];
    }

    return nullptr;
}


std::shared_ptr<Mesh> Reader::GetMesh(size_t index)
{
    if (index < meshes_.size())
    {
        return meshes_[index];
    }

    return nullptr;
}


std::shared_ptr<Mesh> Reader::GetMesh(G4String /*name*/)
{
    //TODO: Return mesh based on its name.

    return nullptr;    
}


Meshes Reader::GetMeshes()
{
    return meshes_;
}


size_t Reader::AddMesh(std::shared_ptr<Mesh> mesh)
{
    meshes_.push_back(mesh);

    return meshes_.size();
}


void Reader::SetMeshes(Meshes meshes)
{
    meshes_ = meshes;
}


}

}

