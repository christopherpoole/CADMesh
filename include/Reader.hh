
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
    std::shared_ptr<Mesh> GetMesh(G4String name);

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

