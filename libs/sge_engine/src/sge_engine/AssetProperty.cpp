#include "AssetProperty.h"
#include "sge_engine/TypeRegister.h"
#include "sge_core/ICore.h"

namespace sge {

bool AssetProperty::update() {
	if (isUpToDate())
		return false;

	m_currentAsset = m_targetAsset;
	if (m_currentAsset.empty() == false) {
		m_modelAsset = getCore()->getAssetLib()->getAsset(m_assetType, m_currentAsset.c_str(), true);
	} else {
		m_modelAsset = std::shared_ptr<Asset>();
	}

	return true;
}

AssetModel* AssetProperty::getAssetModel() {
	if (isAssetLoaded(m_modelAsset) == false || m_modelAsset->getType() != AssetType::Model) {
		return nullptr;
	}

	return m_modelAsset->asModel();
}

const AssetModel* AssetProperty::getAssetModel() const {
	if (isAssetLoaded(m_modelAsset) == false || m_modelAsset->getType() != AssetType::Model) {
		return nullptr;
	}
	return m_modelAsset->asModel();
}

GpuHandle<Texture>* AssetProperty::getAssetTexture() {
	if (isAssetLoaded(m_modelAsset) == false || m_modelAsset->getType() != AssetType::TextureView) {
		return nullptr;
	}

	return m_modelAsset->asTextureView();
}

const GpuHandle<Texture>* AssetProperty::getAssetTexture() const {
	if (isAssetLoaded(m_modelAsset) == false || m_modelAsset->getType() != AssetType::TextureView) {
		return nullptr;
	}
	return m_modelAsset->asTextureView();
}

void AssetProperty::setAsset(std::shared_ptr<Asset>& asset) {
	if (asset && asset->getType() == m_assetType) {
		m_modelAsset = asset;
		m_currentAsset = m_modelAsset->getPath();
	}
	else {
		sgeAssert(false);
	}
}

void AssetProperty::setTargetAsset(const char* const assetPath) {
	m_targetAsset = assetPath ? assetPath : "";
}

DefineTypeId(AssetProperty, 20'03'01'0001);
ReflBlock() {
	ReflAddType(AssetProperty) ReflMember(AssetProperty, m_targetAsset);
}

} // namespace sge
