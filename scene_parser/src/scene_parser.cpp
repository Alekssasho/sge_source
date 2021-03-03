#include <memory>

#include "sge_utils/utils/FileStream.h"
#include "sge_utils/utils/Path.h"
#include "sge_core/model/ModelWriter.h"

#include "ArgParser.h"

#include "FBXSDKParser.h"

using namespace sge;

bool convertModel(
	std::string outputFilename,
	const std::string& inputFilename,
	const ModelParseSettings& traverseSetings,
	std::vector<std::string>* referencedTextures)
{
	const auto saveModelToFile = [&inputFilename](const Model::Model& modelToSave, const char* const outputFilename) -> bool {
		
		createDirectory(extractFileDir(outputFilename, false).c_str());
		ModelWriter modelWriter;
		const bool succeeded = modelWriter.write(modelToSave, outputFilename);
		if(succeeded == false) {
			printf("Failed to write %s to %s!\n", inputFilename.c_str(), outputFilename);
		}

		return succeeded;
	};

	if(InitializeFBXSDK() == false) {
		return false;
	}
	
	const std::string inputFileExt = extractFileExtension(inputFilename.c_str());

	// Import with FBX SDK.
	if(inputFileExt == "fbx" || inputFileExt == "dae")
	{
		printf("Waiting for FBX SDK...\n");

		static fbxsdk::FbxManager* fbxMngr = fbxsdk::FbxManager::Create();

		fbxsdk::FbxIOSettings* fbxIOSets = fbxsdk::FbxIOSettings::Create(fbxMngr, IOSROOT);
		fbxMngr->SetIOSettings(fbxIOSets);
		
		FbxImporter* fbxImporter = FbxImporter::Create(fbxMngr, "");
		
		const bool fbxImporterInitialized = fbxImporter->Initialize(inputFilename.c_str(), -1, fbxMngr->GetIOSettings());
		if(fbxImporterInitialized)
		{
			FbxScene* const fbxScene = FbxScene::Create(fbxMngr, "fbxsdkImportedScene");

			// In that mode export every sub-node of the root node as a separate model.
			const bool multiExportMode = inputFilename.find(".multi") != std::string::npos;
			if(multiExportMode) {
				outputFilename.erase(outputFilename.find(".multi"), strlen(".multi"));
			}

			if(fbxImporter->Import(fbxScene)) {
				if(multiExportMode) {
					FbxNode* const fbxRoot = fbxScene->GetRootNode();
					for(int t = 0; t < fbxRoot->GetChildCount(); ++t) {
						FbxNode* const fbxChild = fbxRoot->GetChild(t);

						// Append the name of the node in the result.
						const std::string newExt = string_format("%s.mdl", fbxChild->GetName());
						const std::string newOutName = replaceExtension(outputFilename.c_str(), newExt.c_str());

						FBXSDKParser fbxsdkParser;
						Model::Model importedModel;

						if(fbxsdkParser.parse(&importedModel, referencedTextures, fbxScene, fbxChild, traverseSetings)) {
							saveModelToFile(importedModel, newOutName.c_str());
						} else {
							printf("Something went wrong when converting %s ot  %s", inputFilename.c_str(), outputFilename.c_str());
							return false;
						}
					}
				} else {
					FBXSDKParser fbxsdkParser;
					Model::Model importedModel;

					if(fbxsdkParser.parse(&importedModel, referencedTextures, fbxScene, nullptr, traverseSetings)) {
						saveModelToFile(importedModel, outputFilename.c_str());
					} else {
						printf("Something went wrong when converting %s ot  %s", inputFilename.c_str(), outputFilename.c_str());
						return false;

					}
				}
			}
		}
		else
		{
			printf("FBX Importer failed to initialize for scene '%s'\n", inputFilename.c_str());
		}
	}

	return true;
}

//-----------------------------------------------------------
//
//-----------------------------------------------------------
int main(int argc, const char* argv[])
{
	const ArgParser argParser(argc, argv);
	if(argParser.isFailed()) {
		return 1;
	}

	if(argParser.getMode() == ProgramMode::Mesh)
	{
		printf("ProgramMode::Mesh");
		const ArgParser::MeshModeArguments& args = argParser.getMeshModeArgs();
		if(convertModel(args.output, args.input, args.parseSettings, nullptr) == false) {
			return 1;
		}
	}
	else if(argParser.getMode() == ProgramMode::Content)
	{
		const ArgParser::ContentModeArguments& args = argParser.getContentModeArgs();

		printf("Content file: %s\n", args.contentFilePath.c_str());
		printf("Assets input directory: %s\n", args.assetsInputDir.c_str());
		printf("Assets output directory: %s\n", args.outputDir.c_str());

		const std::string absoluteConfigFilePath = [&]() -> std::string
		{
			if(isPathAbsolute(args.contentFilePath.c_str())) return args.contentFilePath;
			return currentWorkingDir() + "/" + args.contentFilePath;
		}();
		
		JsonParser jsonParser;

		FileReadStream frs(absoluteConfigFilePath.c_str());
		if(!frs.isOpened() || !jsonParser.parse(&frs)){
			printf("Unable to open the content description file.");
			return 1; // Failed.
		}

		frs.close();

		const std::string absoluteCfgFileDir = extractFileDir(absoluteConfigFilePath.c_str(), true);

		auto const jRoot = jsonParser.getRigidBody();
		auto const jMeshes = jRoot->getMember("meshes");
		auto const jFiles = jRoot->getMember("files");

		auto getAbsolutePathFromCfg = [&](const JsonValue* const jVal) -> std::string
		{
			const char* const path = jVal ? jVal->GetString() : ""; // or should it be "./" instead of ""
			if(isPathAbsolute(path)) return canonizePathRespectOS(std::string(path) + "/");
			return canonizePathRespectOS(absoluteCfgFileDir + path+ + "/");
		};

		const std::string assetsInputDir = canonizePathRespectOS(args.assetsInputDir + "/");
		const std::string assetOutputDir = canonizePathRespectOS(args.outputDir + "/");

		const ContentParsingSettings parseSettings(assetsInputDir, assetOutputDir);
		//RelativeAssetRelocationPolicy relocationPolicy = RelativeAssetRelocationPolicy(parseSettings);
		NoneAssetRelocationPolicy relocationPolicy = NoneAssetRelocationPolicy();

		// Parse the meshes
		for(const JsonValue* const jMesh : jMeshes->arr())
		{
			const auto jIn =  jMesh->getMember("in");
			const auto jAssetName =  jMesh->getMember("assetName");
			const auto jNoAnim = jMesh->getMember("noanim");
			
			const std::string inJson = jIn->GetString();
			
			// if the asset name isn't specified, use the in one and replace theextension
			std::string assetName = jAssetName ? jAssetName->GetString() : "";
			if(assetName.empty()) {
				assetName = replaceExtension(inJson.c_str(), "mdl");
			}

			// In order to maintain the hierarcy of the input assets we use the asset name as a path. This is by design.
			const std::string assetNameDir = extractFileDir(assetName.c_str(), true);

			const bool isInJsonAbsolute = isPathAbsolute(inJson.c_str());

			const std::string inAbsolute = isInJsonAbsolute ? inJson : assetsInputDir + inJson;
			const std::string outAbsolute = assetOutputDir + assetName;

			const ModelParseSettings sceneParseSettings(
				(jNoAnim) ? jNoAnim->getAsBool() : true,
				MeshPacking::NoPacking, 
				assetNameDir, 
				&relocationPolicy);
			
			std::vector<std::string> referencedTextures;

			printf("Converting mesh '%s' \n", inAbsolute.c_str());

			convertModel(outAbsolute, inAbsolute, sceneParseSettings, &referencedTextures);

			for(const auto& texPath : referencedTextures) {
				const std::string input = extractFileDir(inAbsolute.c_str(), true) + texPath;
				const std::string output = extractFileDir(outAbsolute.c_str(), true) + texPath;

				printf("Copying referenced texture '%s' to '%s'\n", input.c_str(), output.c_str());
				copyFile(input.c_str(), output.c_str());
			}
		}

		if(jFiles)
		for(const JsonValue* const jFile : jFiles->arr())
		{
			const auto jAssetName = jFile->getMember("assetName");

			const std::string inJson = jFile->getMember("in")->GetString();

			const bool isInJsonAbsolute = isPathAbsolute(inJson.c_str());

			const std::string inAbsolute = isInJsonAbsolute ? inJson : assetsInputDir + inJson;
			
			std::string outAbsolute;
			if(jAssetName) outAbsolute = assetOutputDir + jAssetName->GetString();
			else outAbsolute = assetOutputDir + inJson;

			createDirectory(extractFileDir(outAbsolute.c_str(), true).c_str());

			copyFile(inAbsolute.c_str(), outAbsolute.c_str());
		}
	}
	else
	{
		printf("Unknown mode!\n");
	}

	return 0;
}
