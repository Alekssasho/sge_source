
cmake_minimum_required(VERSION 3.5)

# Create a target for the editor itself.
add_dir_rec_2(SGE_PROJ_MAKER_SOURCES "./src" 1)
add_executable(project_maker WIN32 ${SGE_PROJ_MAKER_SOURCES}) 
target_link_libraries(project_maker sge_core)

sgePromoteWarningsOnTarget(project_maker)

# Visual Studio Debugging		   
set_target_properties(project_maker PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_INSTALL_PREFIX}/$<CONFIG>/binaries")
	
set_target_properties(project_maker PROPERTIES 
    VS_DEBUGGER_COMMAND "${CMAKE_INSTALL_PREFIX}/$<CONFIG>/binaries/$<TARGET_FILE_NAME:project_maker>")

# Copy the file to install dir avoid installing every time (simplifying debugging).
add_custom_command(TARGET project_maker POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       $<TARGET_FILE:project_maker>
                       ${CMAKE_INSTALL_PREFIX}/$<CONFIG>/binaries)