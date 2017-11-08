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
#include "Exceptions.hh"


namespace CADMesh
{

template <typename T>
CADMeshTemplate<T>::CADMeshTemplate() : CADMeshTemplate<T>("")
{
}

    
template <typename T>
CADMeshTemplate<T>::CADMeshTemplate(G4String file_name)
        : CADMeshTemplate<T>(file_name, File::Unknown)
{
}


template <typename T>
CADMeshTemplate<T>::CADMeshTemplate( G4String file_name, File::Type file_type)
        : CADMeshTemplate<T>(file_name, file_type, CADMESH_DEFAULT_READER)
{
}

    
template <typename T>
CADMeshTemplate<T>::CADMeshTemplate( std::shared_ptr<File::Reader> reader)
        : CADMeshTemplate<T>("", reader)
{
}

    
template <typename T>
CADMeshTemplate<T>::CADMeshTemplate( G4String file_name
                                   , std::shared_ptr<File::Reader> reader)
        : CADMeshTemplate<T>(file_name, File::Unknown, reader)
{
}


template <typename T>
CADMeshTemplate<T>::CADMeshTemplate( G4String file_name
                                   , File::Type file_type
                                   , std::shared_ptr<File::Reader> reader)
{
    if (!reader->CanRead(file_type))
    {
        // TODO: Check that the file type exists in the type name map.
        Exceptions::ReaderCantReadError( reader->GetName()
                                       , file_type
                                       , file_name);
    }

    file_name_ = file_name;
    file_type_ = file_type;

    scale_ = mm;

    offset_ = G4ThreeVector();

    verbose_ = 0;

    reader_ = reader;

    reader_->Read(file_name_);
}


template <typename T>
std::shared_ptr<T> CADMeshTemplate<T>::FromPLY(G4String file_name)
{
    return std::make_shared<T>(file_name, File::PLY);
}

    
template <typename T>
std::shared_ptr<T> CADMeshTemplate<T>::FromPLY( G4String file_name
                                              , std::shared_ptr<File::Reader> reader)
{
    return std::make_shared<T>(file_name, File::PLY, reader);
}


template <typename T>
std::shared_ptr<T> CADMeshTemplate<T>::FromSTL(G4String file_name)
{
    return std::make_shared<T>(file_name, File::STL);
}

    
template <typename T>
std::shared_ptr<T> CADMeshTemplate<T>::FromSTL( G4String file_name
                                              , std::shared_ptr<File::Reader> reader)
{
    return std::make_shared<T>(file_name, File::STL, reader);
}


template <typename T>
CADMeshTemplate<T>::~CADMeshTemplate()
{
}


template <typename T>
bool CADMeshTemplate<T>::IsValidForNavigation()
{
    return reader_->GetMesh()->IsValidForNavigation();
}


template <typename T>
G4String CADMeshTemplate<T>::GetFileName()
{
    return file_name_;
}


template <typename T>
File::Type CADMeshTemplate<T>::GetFileType()
{
    return file_type_;
}


template <typename T>
void CADMeshTemplate<T>::SetVerbose(G4int verbose)
{
    verbose_ = verbose;
}


template <typename T>
G4int CADMeshTemplate<T>::GetVerbose()
{
    return verbose_;
}


template <typename T>
void CADMeshTemplate<T>::SetScale(G4double scale)
{
    scale_ = scale;
}


template <typename T>
G4double CADMeshTemplate<T>::GetScale()
{
    return scale_;
}


template <typename T>
void CADMeshTemplate<T>::SetOffset(G4ThreeVector offset)
{
    offset_ = offset;
}


template <typename T>
G4ThreeVector CADMeshTemplate<T>::GetOffset()
{
    return offset_;
}


} // CADMesh namespace

