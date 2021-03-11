project(Demo)

cmake_minimum_required(VERSION 3.14)

add_library(Demo SHARED src/PluginMain.cpp)
set_target_properties(Demo PROPERTIES SUFFIX ".gll")

target_link_libraries(Demo PUBLIC sge_core)
target_link_libraries(Demo PUBLIC sge_engine)
target_link_libraries(Demo PUBLIC sge_renderer)
target_link_libraries(Demo PUBLIC sge_utils)

sge_generate_assemble_and_debug_target_for_game(Demo ${CMAKE_SOURCE_DIR}/output/Demo)

