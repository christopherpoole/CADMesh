#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(tetrahedral_mesh)

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
#
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include 
                    ${Geant4_INCLUDE_DIR})
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(cadmesh-tetrahedral-mesh cadmesh-tetrahedral-mesh.cc ${sources} ${headers})
target_link_libraries(cadmesh-tetrahedral-mesh ${Geant4_LIBRARIES} cadmesh)

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build this example. This is so that we can run the executable directly
# because it relies on these scripts being in the current working directory.
#
set(TETRAHEDRAL_MESH_FILES BodyHasHeart.ele BodyHasHeart.node visual_run.mac)

foreach(_file ${TETRAHEDRAL_MESH_FILES})
  configure_file(${PROJECT_SOURCE_DIR}/${_file}
                 ${PROJECT_BINARY_DIR}/${_file}
                 COPYONLY)
endforeach()
