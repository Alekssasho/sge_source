#
# sge_engine library
#
cmake_minimum_required(VERSION 3.5)

add_dir_rec_2(SOURCES_ENGINE "./src" 2)

add_library(sge_engine SHARED ${SOURCES_ENGINE})
target_compile_definitions(sge_engine PRIVATE SGE_ENGINE_BUILDING_DLL=1)

target_include_directories(sge_engine PUBLIC "./src")
target_include_directories(sge_engine PUBLIC "../../libs_ext/bullet/bullet3/src")
target_link_libraries(sge_engine sge_core mdlconvlib BulletDynamics BulletCollision LinearMath Recast Detour)
if(WIN32)
	target_link_libraries(sge_engine Dbghelp.lib)
endif()

sgePromoteWarningsOnTarget(sge_engine)



