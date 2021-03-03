cmake_minimum_required(VERSION 3.5)

#############################################################
# Recast
#############################################################
set(RECAST_SRC_FILES
	recastnavigation/Recast/Include/Recast.h
	recastnavigation/Recast/Include/RecastAlloc.h
	recastnavigation/Recast/Include/RecastAssert.h
	
	recastnavigation/Recast/Source/Recast.cpp
	recastnavigation/Recast/Source/RecastAlloc.cpp
	recastnavigation/Recast/Source/RecastArea.cpp
	recastnavigation/Recast/Source/RecastContour.cpp
	recastnavigation/Recast/Source/RecastFilter.cpp
	recastnavigation/Recast/Source/RecastLayers.cpp
	recastnavigation/Recast/Source/RecastMesh.cpp
	recastnavigation/Recast/Source/RecastMeshDetail.cpp
	recastnavigation/Recast/Source/RecastRasterization.cpp
	recastnavigation/Recast/Source/RecastRegion.cpp
)

add_library(Recast STATIC ${RECAST_SRC_FILES})
target_include_directories(Recast PUBLIC recastnavigation/Recast/Include/)

#############################################################
# Detour
#############################################################
set(DETOUR_SRC_FILES
	recastnavigation/Detour/Include/DetourAlloc.h
	recastnavigation/Detour/Include/DetourAssert.h
	recastnavigation/Detour/Include/DetourCommon.h
	recastnavigation/Detour/Include/DetourMath.h
	recastnavigation/Detour/Include/DetourNavMesh.h
	recastnavigation/Detour/Include/DetourNavMeshBuilder.h
	recastnavigation/Detour/Include/DetourNavMeshQuery.h
	recastnavigation/Detour/Include/DetourNode.h
	recastnavigation/Detour/Include/DetourStatus.h
	
	recastnavigation/Detour/Source/DetourAlloc.cpp
	recastnavigation/Detour/Source/DetourCommon.cpp
	recastnavigation/Detour/Source/DetourNavMesh.cpp
	recastnavigation/Detour/Source/DetourNavMeshBuilder.cpp
	recastnavigation/Detour/Source/DetourNavMeshQuery.cpp
	recastnavigation/Detour/Source/DetourNode.cpp
)

add_library(Detour STATIC ${DETOUR_SRC_FILES})
target_include_directories(Detour PUBLIC recastnavigation/Detour/Include/)

	