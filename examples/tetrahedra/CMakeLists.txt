cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(tetrahedra)

set(CMAKE_BUILD_TYPE Debug)

find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/../../)

# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! #
# !!! You need to add TETGEN to your project. !!! #
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! #
find_package(tetgen)

file(COPY ${PROJECT_SOURCE_DIR}/../../meshes/sphere.ply DESTINATION ${CMAKE_BINARY_DIR}/)

add_executable(tetrahedra tetrahedra.cc)
target_link_libraries(tetrahedra ${Geant4_LIBRARIES})

# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! #
# !!! Don't forget to link the TETGEN libraries. !!! #
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! #
target_link_libraries(tetrahedra tet)

