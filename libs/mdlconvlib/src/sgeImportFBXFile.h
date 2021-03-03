#pragma once

#include "ModelParseSettings.h"
#include "sge_core/model/Model.h"

#if WIN32
#ifdef SGE_MDLCONVLIB_BUILDING_DLL
#define SGE_MDLCONVLIB_API __declspec(dllexport)
#else
#define SGE_MDLCONVLIB_API __declspec(dllimport)
#endif
#else
#ifdef SGE_MDLCONVLIB_BUILDING_DLL
#define SGE_MDLCONVLIB_API __attribute__((visibility("default")))
#else
#define SGE_MDLCONVLIB_API
#endif
#endif

namespace sge {

/// Imports the specified FBX file into our own internal format.
/// FBX SDK isn't very permissive in it license, as you need a written approval for distributing FBX SDK binaries from Autodesk.
/// We have our own format so we do not need FBX SDK in the working game. However if we want to import FBX files as assets we still need the
/// SDK. By having a DLL that can import FBX files the engine could check if the dll is present and invoke FBX SDK without explicity
/// depending on it at link time. If the SDK is not available we can just say to the engine user that the FBX SDK importer isn't aviable.
/// @param [out] result stores the imported model.
/// @param [in] fbxFilename is the filename to be loaded.
/// @param [in] parseSettings some settings about how the 3D model needs to be imported.
/// @param [out] pOutReferencedTextures A list of referenced textures in the specified filename (used for dependancy tracking).
/// @return true if the import was successful.
SGE_MDLCONVLIB_API bool sgeImportFBXFile(Model::Model& result,
                                         const char* fbxFilename,
                                         const ModelParseSettings& parseSettings,
                                         std::vector<std::string>* pOutReferencedTextures);
} // namespace sge
