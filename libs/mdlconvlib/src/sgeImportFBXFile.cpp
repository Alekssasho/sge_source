#include "sgeImportFBXFile.h"
#include "FBXSDKParser.h"
#include "IAssetRelocationPolicy.h"
#include "ModelParseSettings.h"
#include <type_traits>

using namespace sge;

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

extern "C" {

SGE_MDLCONVLIB_API bool sgeImportFBXFile(Model::Model& result, const char* fbxFilename, std::vector<std::string>* pOutReferencedTextures) {
	// Ensure that the typedef we provide in the header matches the actual function type.
	static_assert(std::is_same<decltype(&sgeImportFBXFile), sgeImportFBXFileFn>::value);

	static fbxsdk::FbxManager* const fbxMngr = fbxsdk::FbxManager::Create();
	FbxImporter* const fbxImporter = FbxImporter::Create(fbxMngr, ""); // todo: do we need a name here?
	const bool fbxImporterInitialized = fbxImporter->Initialize(fbxFilename, -1, fbxMngr->GetIOSettings());

	if (!fbxImporterInitialized) {
		return false;
	}

	FbxScene* const fbxScene = FbxScene::Create(fbxMngr, "fbxsdkImportedScene");
	if (!fbxImporter->Import(fbxScene)) {
		return false;
	}

	ModelParseSettings mps;
	NoneAssetRelocationPolicy relocationPolicy = NoneAssetRelocationPolicy();
	mps.pRelocaionPolicy = &relocationPolicy;

	FBXSDKParser fbxsdkParser;
	if (!fbxsdkParser.parse(&result, pOutReferencedTextures, fbxScene, nullptr, mps)) {
		return false;
	}

	return true;
}

} // extern "C"
