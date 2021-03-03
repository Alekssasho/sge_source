#include "TraitMultiModel.h"

namespace sge {
// clang-format off
DefineTypeId(TraitMultiModel::Element, 20'03'01'0005);
DefineTypeId(std::vector<TraitMultiModel::Element>, 20'03'01'0006);
DefineTypeId(TraitMultiModel, 20'03'01'0007);

ReflBlock() {
	ReflAddType(TraitMultiModel::Element)
		ReflMember(TraitMultiModel::Element, assetProperty);

	ReflAddType(std::vector<TraitMultiModel::Element>);

	ReflAddType(TraitMultiModel);
}

AABox3f TraitMultiModel::getBBoxOS() const {
	AABox3f bboxCombined;

	for (const auto& model : models) {
		const AssetModel* const assetModel = model.assetProperty.getAssetModel();
		if (assetModel && assetModel->staticEval.isInitialized()) {
			AABox3f bbox = assetModel->staticEval.aabox.getTransformed(model.additionalTransform);
			bboxCombined.expand(bbox);
		}
	}

	return bboxCombined; // Return an empty box.
}
// clang-format on
} // namespace sge
