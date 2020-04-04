// The MIT License (MIT)
//
// Copyright (c) 2011-2020 Christopher M. Poole <mail@christopherpoole.net>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

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
        : CADMeshTemplate<T>(file_name, file_type, File::CADMESH_DEFAULT_READER())
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
        Exceptions::ReaderCantReadError( reader->GetName()
                                       , file_type
                                       , file_name);
    }

    file_name_ = file_name;
    file_type_ = file_type;

    scale_ = 1.0;

    offset_ = G4ThreeVector();

    verbose_ = 0;

    reader_ = reader;

    reader_->Read(file_name_);
}

    
template <typename T>
std::shared_ptr<T> CADMeshTemplate<T>::From(G4String file_name)
{
    return std::make_shared<T>(file_name);
}

    
template <typename T>
std::shared_ptr<T> CADMeshTemplate<T>::From( G4String file_name
                                           , std::shared_ptr<File::Reader> reader)
{
    return std::make_shared<T>(file_name, reader);
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
std::shared_ptr<T> CADMeshTemplate<T>::FromOBJ(G4String file_name)
{
    return std::make_shared<T>(file_name, File::OBJ);
}

    
template <typename T>
std::shared_ptr<T> CADMeshTemplate<T>::FromOBJ( G4String file_name
                                              , std::shared_ptr<File::Reader> reader)
{
    return std::make_shared<T>(file_name, File::OBJ, reader);
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
void CADMeshTemplate<T>::SetOffset(G4double x, G4double y, G4double z)
{
    SetOffset(G4ThreeVector(x, y, z));
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

