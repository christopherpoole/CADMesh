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

#pragma once

// GEANT4 //
#include "G4String.hh"

// STL //
#include <map>
#include <algorithm>


namespace CADMesh
{

namespace File
{

enum Type
{
    Unknown,
    PLY,
    STL,
    DAE,
    OBJ,
    TET,
    OFF,
}; 


static std::map<Type, G4String> Extension = {
    { Unknown, "unknown" },
    { PLY, "ply" },
    { STL, "stl" },
    { DAE, "dae" },
    { OBJ, "obj" },
    { TET, "tet" },
    { OFF, "off" }
};


static std::map<Type, G4String> TypeString = {
    { Unknown, "UNKNOWN" },
    { PLY, "PLY" },
    { STL, "STL" },
    { DAE, "DAE" },
    { OBJ, "OBJ" },
    { TET, "TET" },
    { OFF, "OFF" }
};


static std::map<Type, G4String> TypeName = {
    { Unknown, "Unknown File Format" },
    { PLY, "Stanford Triangle Format (PLY)" },
    { STL, "Stereolithography (STL)" },
    { DAE, "COLLADA (DAE)" },
    { OBJ, "Wavefront (OBJ)" },
    { TET, "TetGet (TET)" },
    { OFF, "Object File Format (OFF)" }
};


Type TypeFromExtension(G4String extension);
Type TypeFromName(G4String name);

}

} // CADMesh namespace


