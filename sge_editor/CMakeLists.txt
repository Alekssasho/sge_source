#####################################################
# Project SGE ENGINE
#####################################################

cmake_minimum_required(VERSION 3.5)

# Create a target for the editor itself.
add_dir_rec_2(SGE_EDITOR_SOURCES "./src" 1)
add_executable(sge_editor WIN32 ${SGE_EDITOR_SOURCES}) 
target_link_libraries(sge_editor sge_engine)

if(WIN32)
	# Needed for minidump.
	target_link_libraries(sge_editor Dbghelp.lib)
endif()

sgePromoteWarningsOnTarget(sge_editor)

# Visual Studio Debugging		   
set_target_properties(sge_editor PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/$<CONFIG>/bin")
	
set_target_properties(sge_editor PROPERTIES 
    VS_DEBUGGER_COMMAND "${CMAKE_INSTALL_PREFIX}/$<CONFIG>/bin/$<TARGET_FILE_NAME:sge_editor>")

# Copy the file to install dir avoid installing every time (simplifying debugging).
add_custom_command(TARGET sge_editor POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       $<TARGET_FILE:sge_editor>
                       ${CMAKE_INSTALL_PREFIX}/$<CONFIG>/bin
                   )
