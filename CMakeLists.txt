#------------------------------------------------------------------------------
# project set-up
#
cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(cadmesh)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CADMESH_MAJOR_VERSION 2)
set(CADMESH_MINOR_VERSION 0)
set(CADMESH_PATCH_VERSION rc1)
set(CADMESH_VERSION
    ${CADMESH_MAJOR_VERSION}.${CADMESH_MINOR_VERSION}.${CADMESH_PATCH_VERSION})

include(FindPkgConfig)
include(ExternalProject)

# for "find_package(...)"
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules ${CMAKE_MODULE_PATH})


#------------------------------------------------------------------------------
# dependencies
#

# GEANT4
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
    find_package(Geant4 REQUIRED ui_all vis_all)
else()
    find_package(Geant4 REQUIRED)
endif()
include(${Geant4_USE_FILE})

# Open Asset Importer Library
option(WITH_SYS_ASSIMP "WITH_SYS_ASSIMP" OFF)
if(${WITH_SYS_ASSIMP} MATCHES "ON")
    find_package(assimp)
else()
    ExternalProject_Add(
        assimp_external
        SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v4.0.1
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_BINARY_DIR}
    )
    include_directories(${CMAKE_BINARY_DIR}/include)
endif()

# X11 if on a Mac
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(OPENGL_INCLUDE_DIR_MAC /var/empty/X11/include )
    include_directories(${OPENGL_INCLUDE_DIR_MAC})

    set(OPENGL_INCLUDE_DIR /var/empty/include )
    include_directories(${OPENGL_INCLUDE_DIR})
endif()


#------------------------------------------------------------------------------
# sources, headers, targets
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

include_directories(${PROJECT_SOURCE_DIR}/include)

# Set up the target.
add_library(cadmesh "SHARED" ${sources} ${headers}) 

# mandatory dependence 1: linking to Geant4
target_link_libraries(cadmesh ${Geant4_LIBRARIES})

# mandatory dependence 2: linking to Assimp
if(${WITH_SYS_ASSIMP} MATCHES "OFF")
    add_dependencies(cadmesh assimp_external)
    target_link_libraries(cadmesh -L${CMAKE_BINARY_DIR}/lib assimp)
else()
    target_link_libraries(cadmesh assimp)
endif()


#----------------------------------------------------------------------------
# Examples
#
add_subdirectory(${PROJECT_SOURCE_DIR}/examples/polygon_mesh)
add_subdirectory(${PROJECT_SOURCE_DIR}/examples/tetrahedral_mesh)
add_subdirectory(${PROJECT_SOURCE_DIR}/examples/nested_polygon_mesh)


#------------------------------------------------------------------------------
# Configure the installation
#
configure_file(${PROJECT_SOURCE_DIR}/cadmesh-config.cmake.in cadmesh-config.cmake)
install(FILES ${CMAKE_BINARY_DIR}/cadmesh-config.cmake
        DESTINATION lib/cmake/cadmesh-${CADMESH_VERSION})

# Install the CADMesh headers library.
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)
install(TARGETS cadmesh DESTINATION lib)

# Install the external project libraries
install(DIRECTORY ${CMAKE_BINARY_DIR}/lib/ DESTINATION lib)

# Install the external project headers.
install(DIRECTORY ${CMAKE_BINARY_DIR}/include/ DESTINATION include)

# Set CPack to build an archive.
set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_NAME "cadmesh")
set(CPACK_PACKAGE_VERSION ${CADMESH_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${CADMESH_MAJOR_VERSION})
set(CPACK_PACKAGE_VERSION_MINOR ${CADMESH_MINOR_VERSION})
set(CPACK_PACKAGE_VERSION_PATCH ${CADMESH_PATCH_VERSION})
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)
include(CPack)
