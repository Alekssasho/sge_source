#pragma once

const char* pluginMainCode =
    R"mainPlugin(#include "sge_engine/IPlugin.h"
#include "sge_core/shaders/modeldraw.h"
#include "sge_engine/DefaultGameDrawer.h"

namespace sge
{
	struct PluginGame final : public IPlugin {
		virtual IGameDrawer* allocateGameDrawer() {
			return new DefaultGameDrawer();
		}

		virtual void onLoaded(const InteropPreviousState& prevState, ImGuiContext* imguiCtx, WindowBase* window, ICore* global) {}
		virtual void onUnload(InteropPreviousState& outState) {  }
		virtual void run() {}
		virtual void handleEvent(WindowBase* window, const WindowEvent event, const void* const eventData) {}
	};
}

extern "C" {
#ifdef WIN32
__declspec(dllexport) sge::IPlugin* getInterop() {
	return new sge::PluginGame();
}
#else
__attribute__((visibility("default"))) sge::IPlugin* getInterop() {
	return new sge::PluginGame();
}
#endif
})mainPlugin";

const char* cmakeGameProjectCode =
    R"cmakeCode(project(TemplateGame)

cmake_minimum_required(VERSION 3.14)

# Enable C++17.
if(UNIX)
	# [TODO] remove -w, find out what -g means (i think it instructs NOT to strip debug symbols)
	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w -g -fpermissive") #-std=c++14
endif()
if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif()

if(WIN32)
	set(SGE_REND_API "Direct3D11" CACHE STRING "Rendering API")
else()
	set(SGE_REND_API "OpenGL" CACHE STRING "Rendering API")
endif()
set_property(CACHE SGE_REND_API PROPERTY STRINGS Direct3D11 OpenGL)

if(SGE_REND_API STREQUAL "Direct3D11")
	add_definitions(-DSGE_RENDERER_D3D11)
endif()
if(SGE_REND_API STREQUAL "OpenGL")
	add_definitions(-DSGE_RENDERER_GL)
endif()

set(SGE_ENGINE_DIR "" CACHE PATH "SGE ENGINE")
set(PLUGIN_AVAILABLE_CONFIGS "")

# Available Configurations - Find all existing (already built) configurations in SGE_ENGINE_DIR and make only those available.
# C++ standartd library doesn't work very well if the configurations are mixed, as the memory layout of it's structures might be different.

# NoOpt is a custom configuration based on RelWithDebInfo (basically the same but with no optimizations).
# Take a note that the the available configurations for the plugin depends on which configurations we have already
# buit in the SGE_ENGINE_DIR.
# NoOpt will still be able to configure as RelWithDebInfo is a CMake default configuration and it's options exists
# even if it isn't available.
SET(CMAKE_C_FLAGS_NOOPT ${CMAKE_C_FLAGS_RELWITHDEBINFO})
SET(CMAKE_CXX_FLAGS_NOOPT ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
SET(CMAKE_EXE_LINKER_FLAGS_NOOPT ${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO})
SET(CMAKE_SHARED_LINKER_FLAGS_NOOPT ${CMAKE_SHARED_LINKER_FLAGS_RELWITHDEBINFO})

STRING(REPLACE "-O2" "-O0" CMAKE_C_FLAGS_NOOPT ${CMAKE_C_FLAGS_NOOPT})
STRING(REPLACE "-O2" "-O0" CMAKE_CXX_FLAGS_NOOPT ${CMAKE_CXX_FLAGS_NOOPT})

STRING(REPLACE "/O2" "/Od" CMAKE_C_FLAGS_NOOPT ${CMAKE_C_FLAGS_NOOPT})
STRING(REPLACE "/O2" "/Od" CMAKE_CXX_FLAGS_NOOPT ${CMAKE_CXX_FLAGS_NOOPT})

add_compile_options($<$<CONFIG:NoOpt>:/MD>)

if(EXISTS ${SGE_ENGINE_DIR}/Debug)
	list(APPEND PLUGIN_AVAILABLE_CONFIGS Debug)
endif()

if(EXISTS ${SGE_ENGINE_DIR}/NoOpt)
	list(APPEND PLUGIN_AVAILABLE_CONFIGS NoOpt)
endif()

if(EXISTS ${SGE_ENGINE_DIR}/Release)
	list(APPEND PLUGIN_AVAILABLE_CONFIGS Release)
endif()

if(EXISTS ${SGE_ENGINE_DIR}/RelWithDebInfo)
	list(APPEND PLUGIN_AVAILABLE_CONFIGS RelWithDebInfo)
endif()

set(CMAKE_CONFIGURATION_TYPES ${PLUGIN_AVAILABLE_CONFIGS})

# The plugin(game) target
add_library(TemplateGame SHARED src/PluginMain.cpp)
if(MSVC)
	# Disable a few warnings:
	target_compile_options(TemplateGame PRIVATE "/wd4251") # 'type' : class 'type1' needs to have dll-interface to be used by clients of class 'type2'
	target_compile_options(TemplateGame PRIVATE "/wd4275") # non dll-interface 'type1' used as base for dll-interface 'type2'
endif()


set_target_properties(TemplateGame PROPERTIES SUFFIX ".gll")
target_include_directories(TemplateGame PUBLIC ${SGE_ENGINE_DIR}/include)
target_include_directories(TemplateGame PUBLIC ${SGE_ENGINE_DIR}/include/bullet3)
target_link_directories(TemplateGame PUBLIC ${SGE_ENGINE_DIR}/$<CONFIG>/lib)
target_link_libraries(TemplateGame PUBLIC BulletCollision)
target_link_libraries(TemplateGame PUBLIC BulletDynamics)
target_link_libraries(TemplateGame PUBLIC LinearMath)
target_link_libraries(TemplateGame PUBLIC imgui)
target_link_libraries(TemplateGame PUBLIC sge_core)
target_link_libraries(TemplateGame PUBLIC sge_engine)
target_link_libraries(TemplateGame PUBLIC sge_renderer)
target_link_libraries(TemplateGame PUBLIC sge_utils)

if(SGE_REND_API STREQUAL "OpenGL")
	target_link_libraries(TemplateGame PUBLIC glew)
endif()


FILE(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/game_bin/) 
install(TARGETS TemplateGame DESTINATION ${CMAKE_SOURCE_DIR}/game_bin/)
install(DIRECTORY "${SGE_ENGINE_DIR}/$<CONFIG>/binaries/" DESTINATION "${CMAKE_SOURCE_DIR}/game_bin")

# Copy to avoid installing every time.
add_custom_command(TARGET TemplateGame POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy
                       $<TARGET_FILE:TemplateGame>
                       ${CMAKE_SOURCE_DIR}/game_bin
                   )

# Visual Studio Debugging
set_target_properties(TemplateGame PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/game_bin")
	
set_target_properties(TemplateGame PROPERTIES 
    VS_DEBUGGER_COMMAND "${CMAKE_SOURCE_DIR}/game_bin/sge_editor")
)cmakeCode";
