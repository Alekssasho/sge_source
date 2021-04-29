#
# sge_core
#

cmake_minimum_required(VERSION 3.5)

project(sge_core)

add_dir_rec_2(SOURCES_SGE_ENGINE "./src" 3)

add_library(sge_core SHARED ${SOURCES_SGE_ENGINE})
target_compile_definitions(sge_core PRIVATE SGE_CORE_BUILDING_DLL=1)

target_include_directories(sge_core PUBLIC "./src")
target_include_directories(sge_core PUBLIC ../../libs_ext/stb)

target_link_libraries(sge_core PUBLIC sge_renderer)
target_link_libraries(sge_core PUBLIC sge_audio)
target_link_libraries(sge_core PUBLIC imgui)

if(NOT EMSCRIPTEN)
	#Do not link with our SDL2, Emscripten has it's own.
	target_link_libraries(sge_core PUBLIC SDL2main SDL2)
endif()

sgePromoteWarningsOnTarget(${PROJECT_NAME})
