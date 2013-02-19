cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(cadmesh_example)

# GEANT4 core
find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

# CADMesh
#include_directories(${VCGLIB_DIR})
find_package(cadmesh)
#add_definitions(-DNOVCGLIB -DNOTET)

# User code
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

add_executable(cadmesh_example cadmesh_example.cc ${sources} ${headers})
target_link_libraries(cadmesh_example ${Geant4_LIBRARIES})
target_link_libraries(cadmesh_example ${cadmesh_LIBRARIES})

