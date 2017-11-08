#pragma once

// CADMesh //
#include "Reader.hh"
#include "Lexer.hh"


namespace CADMesh
{

namespace File
{

class BuiltInReader : public Reader
{
  public:
    BuiltInReader() : Reader("BuiltInReader") { };

  public:
    G4bool Read(G4String filepath);
    G4bool CanRead(File::Type file_type);
};

std::shared_ptr<BuiltInReader> BuiltIn();

} // File namespace

} // CADMesh namespace

