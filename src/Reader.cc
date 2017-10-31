
// CADMesh //
#include "Reader.hh"


namespace CADMesh
{

namespace File
{

Reader::Reader()
{
}


Reader::~Reader()
{
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

}

}

