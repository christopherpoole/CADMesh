cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(cadmesh)

include(FindPkgConfig)

# GEANT4
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
    find_package(Geant4 REQUIRED ui_all vis_all)
else()
    find_package(Geant4 REQUIRED)
endif()
include(${Geant4_USE_FILE})

 # QT-Xml
find_package(Qt4 COMPONENTS QtGui QtCore QtXml REQUIRED)
include(${QT_USE_FILE})

# X11 if on a Mac
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(OPENGL_INCLUDE_DIR_MAC /usr/X11/include )
    include_directories(${OPENGL_INCLUDE_DIR_MAC})

    set(OPENGL_INCLUDE_DIR /usr/include )
    include_directories(${OPENGL_INCLUDE_DIR})
ENDIF()

# Tetgen
set(TETGEN_TARGET)
option(WITH_TETGEN "WITH_TETGEN" OFF)
IF(${WITH_TETGEN} MATCHES "ON")
    include_directories(${TETGEN_DIR})
    set(TETGEN_TARGET tet)
ENDIF()
IF(${WITH_TETGEN} MATCHES "OFF")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOTET")
ENDIF()

# Choose between VCGLIB or ASSIMP (default)
option(WITH_ASSIMP "WITH_ASSIMP" ON)
option(WITH_VCGLIB "WITH_VCGLIB" OFF)

# VCGLIB
set(VCGLIB_SRC)
set(VCGLIB_TARGET)
IF(${WITH_VCGLIB} MATCHES "ON")
    include_directories(${VCGLIB_DIR})
    set(VCGLIB_SRC ${VCGLIB_DIR}/wrap/ply/plylib.cpp)
    set(WITH_ASSIMP OFF)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOASSIMP")
    set(VCGLIB_TARGET QtXml)
ENDIF()
IF(${WITH_VCGLIB} MATCHES "OFF")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOVCGLIB")
ENDIF()

# Open Asset Importer Library
IF(${WITH_ASSIMP} MATCHES "ON")
    find_package(assimp)
ENDIF()

# CADMesh
include_directories(include/)

link_directories(${TETGEN_DIR} ${VCGLIB_DIR})
add_library(cadmesh "SHARED" ${PROJECT_SOURCE_DIR}/src/CADMesh.cc ${VCGLIB_SRC}) 
target_link_libraries(cadmesh ${TETGEN_TARGET} ${VCGLIB_TARGET})

IF(${WITH_ASSIMP} MATCHES "ON")
    target_link_libraries(cadmesh assimp)
ENDIF()

target_link_libraries(cadmesh ${Geant4_LIBRARIES})

set (CADMESH_VERSION_MAJOR 0)
set (CADMESH_VERSION_MINOR 9)

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cadmesh-config.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cadmesh-config.cmake" @ONLY IMMEDIATE)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cadmesh-config.cmake" DESTINATION "lib/cmake/cadmesh-${CADMESH_VERSION_MAJOR}.${CADMESH_VERSION_MINOR}")

install(FILES ${PROJECT_SOURCE_DIR}/include/CADMesh.hh DESTINATION include)
install(TARGETS cadmesh DESTINATION lib)

