
// CADMesh //
#include "Exceptions.hh"


namespace CADMesh
{

namespace Exceptions
{

void FileNotFound(G4String origin, G4String filepath)
{
    G4Exception( ("CADMesh in " + origin).c_str()
               , "FileNotFound"
               , FatalException
               , ("The file: \n\t" + filepath + "\ncould not be found.").c_str());
}


} // Exceptions namespace

} // CADMesh namespace
