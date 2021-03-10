macro(game_post_build_assemble target_name target_output_dir)

	add_custom_command(TARGET ${target_name} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${target_name}> ${target_output_dir}/
	)

	add_custom_command(TARGET ${target_name} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sge_editor> ${target_output_dir}/
	)
	
	add_custom_command(TARGET ${target_name} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sge_core> ${target_output_dir}/
	)
	
	add_custom_command(TARGET ${target_name} PRE_BUILD
					   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/libs/sge_core/core_shaders/ ${target_output_dir}/core_shaders)
	
	add_custom_command(TARGET ${target_name} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sge_engine> ${target_output_dir}/
	)
	
	add_custom_command(TARGET ${target_name} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SDL2> ${target_output_dir}/
	)
	
	add_custom_command(TARGET ${target_name} PRE_BUILD
					   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/libs/sge_engine/assets/ ${target_output_dir}/assets)
					   
	# Visual Studio Debugging
	set_target_properties(${target_name} PROPERTIES 
		VS_DEBUGGER_WORKING_DIRECTORY "${target_output_dir}")
		
	set_target_properties(${target_name} PROPERTIES 
		VS_DEBUGGER_COMMAND "${target_output_dir}/$<TARGET_FILE_NAME:sge_editor>")

endmacro()