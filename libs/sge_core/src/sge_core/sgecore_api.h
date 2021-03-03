#pragma once

#if WIN32
	#ifdef SGE_CORE_BUILDING_DLL 
		#define SGE_CORE_API __declspec(dllexport)  
	#else
		#define SGE_CORE_API __declspec(dllimport)  
	#endif
#else
	#ifdef SGE_CORE_BUILDING_DLL 
		#define SGE_CORE_API __attribute__((visibility("default"))) 
	#else
		#define SGE_CORE_API
	#endif
#endif