#pragma once

#include "sge_core/AssetLibrary.h"
#include "sge_engine_api.h"

namespace sge {

/// @brief A Helper class usually used as a property to game object.
/// This is a reference to an asset, by using the @update() method you can check if new asset has been assigned.
struct SGE_ENGINE_API AssetProperty {
	explicit AssetProperty(AssetType assetType)
	    : m_assetType(assetType) {}

	explicit AssetProperty(AssetType assetType, const char* const initialTargetAsset)
	    : m_assetType(assetType) {
		setTargetAsset(initialTargetAsset);
	}

	/// @brief Should be called by the user. Checks if the target asset has changed and if its loads the new asset.
	/// @return True if new asset was assigned.
	bool update();

	bool isUpToDate() const { return m_targetAsset == m_currentAsset; }

	void clear() {
		m_currentAsset.clear();
		m_asset = nullptr;
	}

	void setAsset(std::shared_ptr<Asset>& asset);
	void setTargetAsset(const char* const assetPath);

	AssetModel* getAssetModel();
	const AssetModel* getAssetModel() const;
	GpuHandle<Texture>* getAssetTexture();
	const GpuHandle<Texture>* getAssetTexture() const;

	AssetProperty& operator=(const AssetProperty& ref) {
		m_targetAsset = ref.m_targetAsset;
		m_assetType = ref.m_assetType;

		return *this;
	}

  public:
	// Caution: there is a custom copy logic.
	AssetType m_assetType;
	std::string m_targetAsset;

	std::string m_currentAsset;
	std::shared_ptr<Asset> m_asset;
	std::vector<std::string> m_uiPossibleAssets;
};

} // namespace sge
