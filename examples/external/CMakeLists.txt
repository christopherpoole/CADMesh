cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(external)

set(CMAKE_BUILD_TYPE Debug)

find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/../../)

# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! #
# !!! You need to add ASSIMP to your project. !!! #
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! #
find_package(assimp)

file(COPY ${PROJECT_SOURCE_DIR}/../../meshes/cone.dae DESTINATION ${CMAKE_BINARY_DIR}/)

add_executable(external external.cc)
target_link_libraries(external ${Geant4_LIBRARIES})

# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! #
# !!! Don't forget to link the ASIMP libraries. !!! #
# !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! #
target_link_libraries(external assimp)

