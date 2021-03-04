cmake_minimum_required(VERSION 3.5)

# Create a target for the player.
add_dir_rec_2(SGE_PLAYER_SOURCES "./src" 1)
add_executable(sge_player WIN32 ${SGE_PLAYER_SOURCES}) 
target_link_libraries(sge_player sge_engine)

if(WIN32)
	# Needed for minidump.
	target_link_libraries(sge_player Dbghelp.lib)
endif()

sgePromoteWarningsOnTarget(sge_player)

# Visual Studio Debugging		   
set_target_properties(sge_player PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/$<CONFIG>/binaries")
	
set_target_properties(sge_player PROPERTIES 
    VS_DEBUGGER_COMMAND "${CMAKE_INSTALL_PREFIX}/$<CONFIG>/binaries/$<TARGET_FILE_NAME:sge_player>")

# Copy the file to install dir avoid installing every time (simplifying debugging).
add_custom_command(TARGET sge_player POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       $<TARGET_FILE:sge_player>
                       ${CMAKE_INSTALL_PREFIX}/$<CONFIG>/binaries)
