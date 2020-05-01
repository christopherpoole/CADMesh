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
#include "FileTypes.hh"
#include "Mesh.hh"
#include "Exceptions.hh"

// GEANT4 //
#include "G4String.hh"


namespace CADMesh
{

namespace File
{

class Reader
{
  public:
    Reader(G4String reader_name);
    ~Reader();

    virtual G4bool Read(G4String filepath) = 0;
    virtual G4bool CanRead(Type file_type) = 0;

  public: 
    G4String GetName();

    std::shared_ptr<Mesh> GetMesh();
    std::shared_ptr<Mesh> GetMesh(size_t index);
    std::shared_ptr<Mesh> GetMesh(G4String name, G4bool exact = true);

    size_t GetNumberOfMeshes();

    Meshes GetMeshes();

  protected:
    size_t AddMesh(std::shared_ptr<Mesh> mesh);
    void SetMeshes(Meshes meshs);

  private:
    Meshes meshes_;

    G4String name_ = "";
};

}

}

