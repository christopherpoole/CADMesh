/* ************************************************
 * GEANT4 VCGLIB/CAD INTERFACE
 *
 * File:      CADMesh.hh
 *
 * Author:    Christopher M Poole,
 * Email:     mail@christopherpoole.net
 *
 * Date:      7th March, 2011
 **************************************************/

#ifndef CADMesh_HH
#define CADMesh_HH

// GEANT4 //
#include "G4String.hh"
#include "G4ThreeVector.hh"
#include "G4TessellatedSolid.hh"
#include "G4AssemblyVolume.hh"

#ifndef NOVCGLIB
// VCGLIB //
#include "vcg/simplex/vertex/base.h"
#include "vcg/simplex/vertex/component.h"
#include "vcg/simplex/face/base.h"
#include "vcg/simplex/face/component.h"

#include "vcg/complex/complex.h"
#endif

// TETGEN //
#ifndef NODEF
#include "tetgen.h"
#endif

#ifndef NOVCGLIB
class CADVertex;
class CADFace;

class CADUsedTypes: public vcg::UsedTypes< vcg::Use<CADVertex>::AsVertexType, vcg::Use<CADFace>::AsFaceType>{};

class CADVertex : public vcg::Vertex<CADUsedTypes, vcg::vertex::Coord3d, vcg::vertex::Normal3f> {};
class CADFace : public vcg::Face<CADUsedTypes, vcg::face::VertexRef> {};
class CADTriMesh : public vcg::tri::TriMesh< std::vector<CADVertex>, std::vector<CADFace> > {};
#endif

class CADMesh
{
// METHODS //
public:
    CADMesh(char * file, char * type, double units, G4ThreeVector offset, G4bool reverse);
    CADMesh(char * file, char * type, G4Material * material, double quality);
    CADMesh(char * file, char * type);
    CADMesh(char * file, char * type, G4Material * material);
    ~CADMesh();

#ifndef NOTET
public:
    G4AssemblyVolume * TetrahedralMesh();
    G4AssemblyVolume * GetAssembly() { return assembly; };

    int get_input_point_count() { return in.numberofpoints; };
    int get_output_point_count() { return out.numberofpoints; };
    int get_tetrahedron_count() { return out.numberoftetrahedra; };
#endif

#ifndef NOVCGLIB
public:
    G4VSolid* TessellatedMesh();
    G4TessellatedSolid * GetSolid() { return volume_solid; };

    G4String MeshName(){ return file_name_; };
    int MeshVertexNumber(){ return m.VertexNumber(); };
    float MeshVolume(){ return m.Volume(); };

    void SetVerbose(int v){ verbose_ = v; };
#endif

private:
    G4ThreeVector GetTetPoint(G4int offset);


// VARS //
private:
#ifndef NOVCGLIB
    CADTriMesh m;
#endif

#ifndef NOTET
    tetgenio in, out;
    G4AssemblyVolume * assembly;
#endif

    G4TessellatedSolid * volume_solid;
    G4int verbose_;
    G4bool has_mesh_;
    G4bool has_solid_;

    G4String name_;
    G4double units_;
    G4ThreeVector offset_;
    G4bool reverse_;

    G4double quality_;
    
    G4Material * material_;

    char * file_name_;
    G4String file_type_;
};

#endif // CADMesh_HH

