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
CADMeshTemplate<T>::CADMeshTemplate()
{
    Init();
}


template <typename T>
CADMeshTemplate<T>::CADMeshTemplate(G4String file_name)
{
    Init();
    this->file_name_ = file_name;
}


template <typename T>
CADMeshTemplate<T>::CADMeshTemplate(G4String file_name, G4String file_type)
{
    Init();
    this->file_name_ = file_name;
    this->file_type_ = file_type;
}


template <typename T>
std::shared_ptr<T> CADMeshTemplate<T>::FromPLY(G4String file_name)
{
    return std::make_shared<T>(file_name, "PLY");
}


template <typename T>
CADMeshTemplate<T>::~CADMeshTemplate()
{
}


template <typename T>
void CADMeshTemplate<T>::Init()
{
    this->file_name_ = "";
    this->file_type_ = "";

    this->scale_ = mm;

    this->offset_ = G4ThreeVector();

    this->verbose_ = 0;
}

} // CADMesh namespace

