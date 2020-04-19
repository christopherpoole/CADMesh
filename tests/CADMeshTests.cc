#define CATCH_CONFIG_MAIN 
#include "catch2/catch.hpp"

// CADMesh //
#include "CADMesh.hh"


SCENARIO( "Load a PLY file as a tessellated mesh.") {

    GIVEN( "the PLY file on disk" ) {
        auto mesh = CADMesh::TessellatedMesh::FromPLY("../meshes/sphere.ply");

        REQUIRE( mesh->GetFileName() == "../meshes/sphere.ply" );
        
        WHEN( "setting the output verbosity" ) {
            mesh->SetVerbose(1);
            
            THEN( "the output verbosity flag changes" ) {
                REQUIRE( mesh->GetVerbose() == 1 );
            }
        }
    }
}
