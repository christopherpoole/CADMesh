/********************************************************************************
* CADMesh - A CAD file interface for Geant4                                     *
*                                                                               *
* Copyright (c) 2017      Simon Kirchhof <s.kirchhof@dkfz-heidelberg.de>        *
*                                                                               *
* Permission is hereby granted, free of charge, to any person obtaining a copy  *
* of this software and associated documentation files (the "Software"),         *
* to deal in the Software without restriction, including without limitation     *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,      *
* and/or sell copies of the Software, and to permit persons to whom             *
* the Software is furnished to do so, subject to the following conditions:      *
*                                                                               *
* The above copyright notice and this permission notice shall be included       *
* in all copies or substantial portions of the Software.                        *
*                                                                               *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,               *
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES               *
* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.     *
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   *
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,                 *
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE    *
* OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                 *
********************************************************************************/
#pragma once

#include <map>

#include <G4Types.hh>
#include <G4Colour.hh>
#include <G4Material.hh>


namespace cadmesh {

constexpr G4int DEFAULT_REGION_ID = -666;

struct RegionAttributes
{
  G4Colour colour;
  G4Material* material;
};

typedef std::map<G4int, RegionAttributes> RegionAttributeMap;

inline
void PrintTable(const RegionAttributeMap& regionAttributeMap)
{
  for (const auto& pair : regionAttributeMap)
  {
    G4int regionID = pair.first;
    const RegionAttributes& regionAtt = pair.second;
    G4cout << "\tRegion " << regionID << ":\t";
    G4cout << regionAtt.material->GetName() << "\t";
    G4cout << regionAtt.colour << G4endl;
  }
}

}  // namespace cadmesh
