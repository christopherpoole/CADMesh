/* ************************************************
 * GEANT4 CAD INTERFACE
 *
 * File:      CADMesh.cc
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Date:      7th March, 2011
 **************************************************/

// CADMesh //
#include "CADMeshTemplate.hh"


namespace CADMesh
{

template <typename T>
CADMeshTemplate<T>::CADMeshTemplate() : CADMeshTemplate("")
{
}


template <typename T>
CADMeshTemplate<T>::CADMeshTemplate(G4String file_name)
        : CADMeshTemplate(file_name, File::Unknown)
{
}


template <typename T>
CADMeshTemplate<T>::CADMeshTemplate(G4String file_name, File::Type file_type)
{
    this->file_name_ = file_name;
    this->file_type_ = file_type;

    this->scale_ = mm;

    this->offset_ = G4ThreeVector();

    this->verbose_ = 0;
}


template <typename T>
std::shared_ptr<T> CADMeshTemplate<T>::FromPLY(G4String file_name)
{
    return std::make_shared<T>(file_name, File::PLY);
}


template <typename T>
CADMeshTemplate<T>::~CADMeshTemplate()
{
}

} // CADMesh namespace

