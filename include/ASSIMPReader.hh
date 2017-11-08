/* ************************************************
 * GEANT4 CAD INTERFACE
 *
 * File:      TessellatedMesh.hh
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Original:  7th March, 2011
 **************************************************/

#pragma once

// CADMesh //
#include "Reader.hh"

// Open Asset Importer Library //
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"


namespace CADMesh
{

namespace File
{

class ASSIMPReader : public File::Reader
{
  public:
    ASSIMPReader();
    ~ASSIMPReader();

  public:
    G4bool Read(G4String filepath);
    G4bool CanRead(Type file_type);

  private:
    Assimp::Importer* importer_;
};

std::shared_ptr<ASSIMPReader> ASSIMP();

} // File namespace

} // CADMesh namespace

