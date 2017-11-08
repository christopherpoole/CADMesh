
// CADMesh //
#include "BuiltInReader.hh"
#include "STLReader.hh"


namespace CADMesh
{

namespace File
{

G4bool BuiltInReader::Read(G4String filepath)
{
    File::Reader* reader = nullptr;

    // TODO: Guess the file type.
    reader = new File::STLReader();

    if (!reader->Read(filepath))
    {
        return false;
    }

    SetMeshes(reader->GetMeshes());
    return true;
}


G4bool BuiltInReader::CanRead(Type file_type)
{
    if (file_type == STL)
    {
        return true;
    }

    return false;
}


std::shared_ptr<BuiltInReader> BuiltIn()
{
    return std::make_shared<BuiltInReader>();
}

} // File namespace 

} // CADMesh namespace

