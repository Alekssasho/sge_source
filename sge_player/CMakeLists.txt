cmake_minimum_required(VERSION 3.5)

# Create a target for the player.
add_dir_rec_2(SGE_PLAYER_SOURCES "./src" 1)
if(EMSCRIPTEN)
	add_library(sge_player STATIC ${SGE_PLAYER_SOURCES})
else()
	add_executable(sge_player WIN32 ${SGE_PLAYER_SOURCES}) 
endif()

target_link_libraries(sge_player sge_engine)

if(WIN32)
	# Needed for minidump.
	target_link_libraries(sge_player Dbghelp.lib)
endif()

sgePromoteWarningsOnTarget(sge_player)
