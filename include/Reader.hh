
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
    Reader();
    ~Reader();
    
  public: 
    std::shared_ptr<Mesh> GetMesh();
    std::shared_ptr<Mesh> GetMesh(size_t index);
    std::shared_ptr<Mesh> GetMesh(G4String name);

    Meshes GetMeshes();

    virtual G4bool Read(G4String filepath) = 0;

  protected:
    size_t AddMesh(std::shared_ptr<Mesh> mesh);

  private:
    Meshes meshes_;
};

}

}

