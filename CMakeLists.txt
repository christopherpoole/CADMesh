cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(cadmesh)

include(FindPkgConfig)
include(ExternalProject)


# GEANT4
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
    find_package(Geant4 REQUIRED ui_all vis_all)
else()
    find_package(Geant4 REQUIRED)
endif()
include(${Geant4_USE_FILE})

# X11 if on a Mac
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(OPENGL_INCLUDE_DIR_MAC /var/empty/X11/include )
    include_directories(${OPENGL_INCLUDE_DIR_MAC})

    set(OPENGL_INCLUDE_DIR /var/empty/include )
    include_directories(${OPENGL_INCLUDE_DIR})
endif()

# Tetgen
option(WITH_SYS_TETGEN "WITH_SYS_TETGEN" OFF)
if(${WITH_SYS_TETGEN} MATCHES "ON")
    find_package(tetgen)
else()
    ExternalProject_Add(
        tetgen_external
        SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/tetgen
        GIT_REPOSITORY https://github.com/christopherpoole/tetgen.git
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
    )
    include_directories(${PROJECT_BINARY_DIR}/external/tetgen/include)
endif()

# Open Asset Importer Library
option(WITH_SYS_ASSIMP "WITH_SYS_ASSIMP" OFF)
if(${WITH_SYS_ASSIMP} MATCHES "ON")
    find_package(assimp)
else()
    ExternalProject_Add(
        assimp_external
        SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/assimp
        GIT_REPOSITORY https://github.com/assimp/assimp.git
        GIT_TAG v3.1.1
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}
    )
    include_directories(${PROJECT_BINARY_DIR}/external/assimp/include)
endif()

# CADMesh
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${CMAKE_INSTALL_PREFIX}/include)

add_library(cadmesh "SHARED" ${sources} ${headers}) 

if(${WITH_SYS_TETGEN} MATCHES "OFF")
    add_dependencies(cadmesh tetgen_external)
endif()

if(${WITH_SYS_TETGEN} MATCHES "OFF")
    add_dependencies(cadmesh assimp_external)
endif()

target_link_libraries(cadmesh ${Geant4_LIBRARIES})
target_link_libraries(cadmesh -L${CMAKE_INSTALL_PREFIX}/lib tet)
target_link_libraries(cadmesh assimp)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cadmesh-config.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cadmesh-config.cmake" @ONLY IMMEDIATE)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cadmesh-config.cmake" DESTINATION "lib/cmake/cadmesh-${CADMESH_VERSION_MAJOR}.${CADMESH_VERSION_MINOR}")

install(FILES ${PROJECT_SOURCE_DIR}/include/CADMesh.hh DESTINATION include)
install(TARGETS cadmesh DESTINATION lib)

