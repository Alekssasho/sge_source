project(Demo)

cmake_minimum_required(VERSION 3.14)

add_dir_rec_2(DEMO_SOURCES "./src" 2)
add_library(Demo SHARED ${DEMO_SOURCES})
set_target_properties(Demo PROPERTIES SUFFIX ".gll")
sgePromoteWarningsOnTarget(Demo)

target_link_libraries(Demo PUBLIC sge_core)
target_link_libraries(Demo PUBLIC sge_engine)
target_link_libraries(Demo PUBLIC sge_renderer)
target_link_libraries(Demo PUBLIC sge_utils)

sge_generate_assemble_and_debug_target_for_game(Demo ${CMAKE_SOURCE_DIR}/output/Demo)

