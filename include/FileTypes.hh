/* ************************************************
 * GEANT4 CAD INTERFACE
 *
 * File:      CADMesh.hh
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Date:      7th March, 2011
 **************************************************/

#pragma once

// STL //
#include <string>
#include <map>


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


static std::map<Type, std::string> Entension = {
    { PLY, "ply" },
    { STL, "stl" },
    { DAE, "dae" },
    { OBJ, "obj" },
    { TET, "tet" },
    { OFF, "off" }
};

}

}

