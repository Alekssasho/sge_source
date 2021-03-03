#############################################################################
# FBXSDK_TARGET - used to import the external prebuild FBX SDK libraries
# and correctly link them to our tergets.
# The tricky one is IMPORTED_LOCATION_NOOPT as it is just release withuot
# any optimizations
#############################################################################
cmake_minimum_required(VERSION 2.8)

project(FBXSDK_TARGET)

# Create an imported target for FBX SDK binaries.
add_library(FBXSDK_TARGET STATIC IMPORTED)
add_library(FBXSDK_XML_TARGET STATIC IMPORTED)
add_library(FBXSDK_ZLIB_TARGET STATIC IMPORTED)
if(WIN32)
	# Windows platform.
	set_target_properties(FBXSDK_TARGET PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${SGE_FBX_SDK_DIR}/include"
		
		IMPORTED_LOCATION_RELEASE        "${SGE_FBX_SDK_DIR}/lib/vs2017/x64/release/libfbxsdk-md.lib"
		IMPORTED_LOCATION_RELWITHDEBINFO "${SGE_FBX_SDK_DIR}/lib/vs2017/x64/release/libfbxsdk-md.lib"
		IMPORTED_LOCATION_NOOPT          "${SGE_FBX_SDK_DIR}/lib/vs2017/x64/release/libfbxsdk-md.lib"
		IMPORTED_LOCATION_DEBUG          "${SGE_FBX_SDK_DIR}/lib/vs2017/x64/debug/libfbxsdk-md.lib")
		
	set_target_properties(FBXSDK_XML_TARGET PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${SGE_FBX_SDK_DIR}/include"
		
		IMPORTED_LOCATION_RELEASE        "${SGE_FBX_SDK_DIR}/lib/vs2017/x64/release/libxml2-md.lib"
		IMPORTED_LOCATION_RELWITHDEBINFO "${SGE_FBX_SDK_DIR}/lib/vs2017/x64/release/libxml2-md.lib"
		IMPORTED_LOCATION_NOOPT          "${SGE_FBX_SDK_DIR}/lib/vs2017/x64/release/libxml2-md.lib"
		IMPORTED_LOCATION_DEBUG          "${SGE_FBX_SDK_DIR}/lib/vs2017/x64/debug/libxml2-md.lib")
		
	set_target_properties(FBXSDK_ZLIB_TARGET PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${SGE_FBX_SDK_DIR}/include"
		
		IMPORTED_LOCATION_RELEASE        "${SGE_FBX_SDK_DIR}/lib/vs2017/x64/release/zlib-md.lib"
		IMPORTED_LOCATION_RELWITHDEBINFO "${SGE_FBX_SDK_DIR}/lib/vs2017/x64/release/zlib-md.lib"
		IMPORTED_LOCATION_NOOPT          "${SGE_FBX_SDK_DIR}/lib/vs2017/x64/release/zlib-md.lib"
		IMPORTED_LOCATION_DEBUG          "${SGE_FBX_SDK_DIR}/lib/vs2017/x64/debug/zlib-md.lib")
else()
	# Linux platform.
	target_link_libraries(FBXSDK_TARGET PROPERTIES 
		INTERFACE_INCLUDE_DIRECTORIES "${SGE_FBX_SDK_DIR}/include"
		
		IMPORTED_LOCATION_RELEASE        "${SGE_FBX_SDK_DIR}/lib/gcc4/x64/release/libfbxsdk.a"
		IMPORTED_LOCATION_RELWITHDEBINFO "${SGE_FBX_SDK_DIR}/lib/gcc4/x64/release/libfbxsdk.a"
		IMPORTED_LOCATION_NOOPT          "${SGE_FBX_SDK_DIR}/lib/gcc4/x64/release/libfbxsdk.a"
		IMPORTED_LOCATION_DEBUG          "${SGE_FBX_SDK_DIR}/lib/gcc4/x64/debug/libfbxsdk.a")

		set_target_properties(FBXSDK_XML_TARGET PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${SGE_FBX_SDK_DIR}/include"
		
		IMPORTED_LOCATION_RELEASE        "${SGE_FBX_SDK_DIR}/lib/gcc4/x64/release/libxml2.lib"
		IMPORTED_LOCATION_RELWITHDEBINFO "${SGE_FBX_SDK_DIR}/lib/gcc4/x64/release/libxml2.lib"
		IMPORTED_LOCATION_NOOPT          "${SGE_FBX_SDK_DIR}/lib/gcc4/x64/release/libxml2.lib"
		IMPORTED_LOCATION_DEBUG          "${SGE_FBX_SDK_DIR}/lib/gcc4/x64/debug/libxml2.lib")
		
	set_target_properties(FBXSDK_ZLIB_TARGET PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${SGE_FBX_SDK_DIR}/include"
		
		IMPORTED_LOCATION_RELEASE        "${SGE_FBX_SDK_DIR}/lib/gcc4/x64/release/libzlib.lib"
		IMPORTED_LOCATION_RELWITHDEBINFO "${SGE_FBX_SDK_DIR}/lib/gcc4/x64/release/libzlib.lib"
		IMPORTED_LOCATION_NOOPT          "${SGE_FBX_SDK_DIR}/lib/gcc4/x64/release/libzlib.lib"
		IMPORTED_LOCATION_DEBUG          "${SGE_FBX_SDK_DIR}/lib/gcc4/x64/debug/libzlib.lib")
endif()

#############################################################################
# scene_parser
#############################################################################
project(scene_parser)

include_directories("./src")

link_directories(${OUTPUT_DIR}/libs)

add_dir_rec_2(SOURCES_SCENE_PARSER "./src" 3)
add_executable(scene_parser ${SOURCES_SCENE_PARSER})
sgePromoteWarningsOnTarget(${PROJECT_NAME})

target_link_libraries(scene_parser sge_core)

if(NOT WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
	target_link_libraries(scene_parser ${CMAKE_PTHREAD_LIBS})
endif()
target_link_libraries(scene_parser FBXSDK_TARGET FBXSDK_XML_TARGET FBXSDK_ZLIB_TARGET)


