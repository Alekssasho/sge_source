#include "FBXSDKParser.h"
#include "sgeImportFBXFile.h"

using namespace sge;

namespace sge {

bool sgeImportFBXFile(Model::Model& result,
                      const char* fbxFilename,
                      const ModelParseSettings& parseSettings,
                      std::vector<std::string>* pOutReferencedTextures) {
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

	FBXSDKParser fbxsdkParser;
	if (!fbxsdkParser.parse(&result, pOutReferencedTextures, fbxScene, nullptr, parseSettings)) {
		return false;
	}

	return true;
}

} // extern "C"
