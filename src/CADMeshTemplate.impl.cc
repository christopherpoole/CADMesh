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
#include "CADMeshTemplate.cc"

#include "TessellatedMesh.hh"
#include "TetrahedralMesh.hh"


namespace CADMesh
{

template class CADMeshTemplate<TessellatedMesh>;
template class CADMeshTemplate<TetrahedralMesh>;

}

