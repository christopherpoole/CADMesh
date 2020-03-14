
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
               , ("\nThe file: \n\t" + filepath + "\ncould not be found.").c_str());
}


void LexerError(G4String origin, G4String message)
{
    G4Exception( ("CADMesh in " + origin).c_str()
               , "LexerError"
               , FatalException
               , ("\nThe CAD file appears to contain incorrect syntax:\n\t" + message).c_str());
}


void ParserError(G4String origin, G4String message)
{
    G4Exception( ("CADMesh in " + origin).c_str()
               , "ParserError"
               , FatalException
               , ("\nThe CAD file appears to contain invalid data:\n\t" + message).c_str());
}


void ReaderCantReadError(G4String origin, File::Type file_type, G4String filepath)
{
    G4Exception( ("CADMesh in " + origin).c_str()
               , "ReaderCantReadError"
               , FatalException
               , ("\nThe the reader can't read files of type '"
                   + File::TypeName[file_type]
                   + ".'\n\tSpecified the incorrect file type (?) for file:\n\t\t"
                   + filepath).c_str());
}



} // Exceptions namespace

} // CADMesh namespace
