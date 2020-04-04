cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(basic)

set(CMAKE_BUILD_TYPE Debug)

find_package(Geant4 REQUIRED ui_all vis_all)
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/../../)

file(COPY ${PROJECT_SOURCE_DIR}/../../meshes/sphere.ply DESTINATION ${CMAKE_BINARY_DIR}/)
file(COPY ${PROJECT_SOURCE_DIR}/../../meshes/bunny.stl DESTINATION ${CMAKE_BINARY_DIR}/)
file(COPY ${PROJECT_SOURCE_DIR}/../../meshes/cow.obj DESTINATION ${CMAKE_BINARY_DIR}/)

add_executable(basic basic.cc)
target_link_libraries(basic ${Geant4_LIBRARIES})


