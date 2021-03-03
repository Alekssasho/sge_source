#pragma once
#include "IImGuiWindow.h"
#include "ModelPreviewWindow.h"
#include "imgui/imgui.h"
#include "sgeImportFBXFile.h"
#include "sge_core/AssetLibrary.h"
#include "sge_utils/utils/DLLHandler.h"
#include <string>

namespace sge {
struct InputState;
struct GameInspector;

//----------------------------------------------------------
// AssetsWindow
//----------------------------------------------------------
struct SGE_ENGINE_API AssetsWindow : public IImGuiWindow {
  private:
	struct AssetImportData {
		bool importFailed = false;
		std::string filename;
		AssetType assetType;
		std::string outputDir;
		std::string outputFilename;

		bool preview = true;
		ModelPreviewWidget modelPreviewWidget;

		PAsset tempAsset;
	};

  public:
	AssetsWindow(std::string windowName, GameInspector& inspector);
	bool isClosed() override { return !m_isOpened; }
	void update(SGEContext* const sgecon, const InputState& is) override;
	const char* getWindowName() const override { return m_windowName.c_str(); }

	void openAssetImport(const std::string& filename);

  private:
	void update_assetImport(SGEContext* const sgecon, const InputState& is);

	bool importAsset(AssetImportData& aid);

  private:
	bool m_isOpened = true;
	GameInspector& m_inspector;
	std::string m_windowName;

	ModelPreviewWidget m_modelPreviewWidget;

	std::shared_ptr<Asset> m_selectedAsset;

	bool shouldOpenImportPopup = false;
	bool m_importPopupIsOpen = false;

	std::filesystem::path m_rightClickedPath;
	PAsset explorePreviewAsset;
	ModelPreviewWidget exploreModelPreviewWidget;
	std::vector<std::string> directoryTree;
	ImGuiTextFilter exploreFilter;

	AssetImportData m_importAssetToImportInPopup;

	std::vector<AssetImportData> m_assetsToImport;

	DLLHandler mdlconvlibHandler;
	sgeImportFBXFileFn sgeImportFBXFile = nullptr;
};
} // namespace sge
