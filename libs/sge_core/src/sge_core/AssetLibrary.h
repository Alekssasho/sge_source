#pragma once

#include <map>
#include <memory>

#include "sgecore_api.h"
#include "sge_core/model/EvaluatedModel.h"
#include "sge_core/model/Model.h"
#include "sge_utils/utils/vector_map.h"
#include "sge_core/Sprite.h"

namespace sge {
struct AudioTrack;
using AudioAsset = std::shared_ptr<AudioTrack>;

struct AssetLibrary;
struct AssetModel {
	Model::Model model;
	EvaluatedModel staticEval;
	EvaluatedModel sharedEval;
};

// Defines all posible asset types.
enum class AssetType : int {
	None,
	Model,       // sge::Model::Model
	TextureView, // sge::GpuHandle<sge::Texture>
	Text,        // A file containing some text (including code).
	Sprite,
	Audio,       // Vorbis encoded audio file. Usually used for background music or longer audio tracks.

	Count,
};

SGE_CORE_API const char* assetType_getName(const AssetType type);

SGE_CORE_API AssetType assetType_fromExtension(const char* const ext, bool includeExternalExtensions);

// Provides an interfaces that is used to allocated a particular asset of a type.
struct SGE_CORE_API IAssetAllocator {
	virtual void* allocate() = 0;
	virtual void deallocate(void* ptr) = 0;
};

// Provides an interfaces that is used to load/unload/ect a particular asset of a type.
struct SGE_CORE_API IAssetFactory {
	virtual void getDependancyList(void* UNUSED(asset), std::vector<std::string>& UNUSED(deps)) {}
	virtual bool load(void* const pAsset, const char* const pPath, AssetLibrary* const mpMngrngr) = 0;
	virtual void unload(void* const pAsset, AssetLibrary* const pMngr) = 0;
};

enum class AssetStatus {
	NotLoaded,
	Loaded,
	LoadFailed,
};

//-------------------------------------------------------
//
//-------------------------------------------------------
struct SGE_CORE_API Asset {
	friend AssetLibrary;

	Asset(void* const pAsset, const AssetType type, const AssetStatus status, const char* pPath)
	    : m_pAsset(pAsset)
	    , m_type(type)
	    , m_status(status)
	    , m_path(pPath) {
		if (m_status == AssetStatus::Loaded) {
			sgeAssert(m_pAsset != nullptr);
		}
	}

	GpuHandle<Texture>* asTextureView() {
		sgeAssert(getType() == AssetType::TextureView);
		return (GpuHandle<Texture>*)m_pAsset;
	}

	SpriteAnimationAsset* asSprite() {
		sgeAssert(getType() == AssetType::Sprite);
		return (SpriteAnimationAsset*)m_pAsset;
	}

        sge::AudioAsset* asAudio() {
		sgeAssert(getType() == AssetType::Audio);
		return (AudioAsset*)m_pAsset;
	}

	const void* asVoid() const { return m_pAsset; }
	AssetModel* asModel() {
		sgeAssert(getType() == AssetType::Model);
		return (AssetModel*)m_pAsset;
	}
	const AssetModel* asModel() const {
		sgeAssert(getType() == AssetType::Model);
		return (AssetModel*)m_pAsset;
	}
	const std::string* asText() const {
		sgeAssert(getType() == AssetType::Text);
		return (const std::string*)m_pAsset;
	}
	const SpriteAnimationAsset* asSprite() const {
		sgeAssert(getType() == AssetType::Sprite);
		return (const SpriteAnimationAsset*)m_pAsset;
	}
	const sge::AudioAsset* asAudio() const {
		sgeAssert(getType() == AssetType::Audio);
		return (const AudioAsset*)m_pAsset;
	}

	AssetType getType() const { return m_type; }
	AssetStatus getStatus() const { return m_status; }
	const std::string& getPath() const { return m_path; }
	sint64 getLastModTime() const { return m_loadedModifiedTime; }

  private:
	void* m_pAsset = nullptr;
	AssetType m_type;
	AssetStatus m_status = AssetStatus::NotLoaded;
	std::string m_path;          // TODO: Or should I use a raw data? Different assets could use different load settings.
	sint64 m_loadedModifiedTime; // The last time the file was modified since last load.
};

using PAsset = std::shared_ptr<Asset>;
using WAsset = std::weak_ptr<Asset>;

//-------------------------------------------------------
// AssetLibrary
//-------------------------------------------------------
struct SGE_CORE_API AssetLibrary {
  public:
	AssetLibrary(SGEDevice* const sgedev);


	/// Make runtime asset
	std::shared_ptr<Asset> makeRuntimeAsset(AssetType type, const char* path);

	// Attempts load an asset. Returns true if new version was found and the asset was reloaded.
	std::shared_ptr<Asset> getAsset(AssetType type, const char* pPath, const bool loadIfMissing);

	/// Retrieves the asset and loading it by guess the Asset Type based on the file extension.
	std::shared_ptr<Asset> getAsset(const char* pPath, const bool loadIfMissing);

	bool loadAsset(Asset* asset);

	// Reloads an asset is the source file modified time has changed.
	bool reloadAssetModified(Asset* const asset);

	IAssetAllocator* getAllocator(const AssetType type) { return m_assetAllocators[type]; }
	IAssetFactory* getFactory(const AssetType type) { return m_assetFactories[type]; }

	SGEDevice* getDevice() { return m_sgedev; }

	std::map<std::string, std::shared_ptr<Asset>>& getAllAssets(AssetType type) {
		const auto& itr = m_assets.find(type);
		sgeAssert(itr != m_assets.end());
		return itr->second;
	}

	const std::map<std::string, std::shared_ptr<Asset>>& getAllAssets(AssetType type) const {
		const auto& itr = m_assets.find(type);
		sgeAssert(itr != m_assets.end());
		return itr->second;
	}

	void scanForAvailableAssets(const char* const path);
	void reloadChangedAssets();

	const std::string& getAssetsDirAbs() const { return m_gameAssetsDir; }

  private:
	std::string m_gameAssetsDir;

	void markThatAssetExists(const char* path, AssetType const type);

	// TODO: Maybe we should make this public in order to support "more" asset types on the go... but who cares!?
	// Registers a new asset type
	void registerAssetType(const AssetType type, IAssetAllocator* const pAllocator, IAssetFactory* const pFactory);

	std::map<AssetType, IAssetAllocator*> m_assetAllocators;
	std::map<AssetType, IAssetFactory*> m_assetFactories;

	// TODO: in order not to automatically unload assets shared_ptr is used. This should be optional. An example emplementation is to
	// use weak_ptr here, and another vector<shared_ptr> that holds those special assets(like characters, menu textures, ect.).
	std::map<AssetType, std::map<std::string, std::shared_ptr<Asset>>> m_assets;

	SGEDevice* m_sgedev;
};

// Some helpers.

inline bool isAssetLoaded(const std::shared_ptr<Asset>& asset) {
	bool loaded = asset && (asset->getStatus() == AssetStatus::Loaded);
	if (loaded) {
		sgeAssert(asset && !!asset->asVoid());
	}
	return loaded;
}

inline bool isAssetLoaded(const std::shared_ptr<Asset>& asset, const AssetType type) {
	bool loaded = asset && (asset->getStatus() == AssetStatus::Loaded);
	if (loaded) {
		sgeAssert(asset && !!asset->asVoid());
	}
	return loaded && asset->getType() == type;
}

inline bool isAssetNotLoadedOrLoadFailed(const std::shared_ptr<Asset>& asset) {
	const bool notLoadedOrFailed = !asset || (asset->getStatus() == AssetStatus::LoadFailed);
	return notLoadedOrFailed;
}

inline bool isAssetLoadFailed(const std::shared_ptr<Asset>& asset) {
	const bool res = asset && (asset->getStatus() == AssetStatus::LoadFailed);
	return res;
}

} // namespace sge
