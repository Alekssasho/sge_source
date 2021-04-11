#include "TraitModel.h"
#include "IconsForkAwesome/IconsForkAwesome.h"
#include "sge_core/SGEImGui.h"
#include "sge_engine/EngineGlobal.h"
#include "sge_engine/GameInspector.h"
#include "sge_engine/GameWorld.h"
#include "sge_engine/TypeRegister.h"
#include "sge_engine/actors/ALocator.h"
#include "sge_engine/materials/Material.h"
#include "sge_engine/windows/PropertyEditorWindow.h"
#include "sge_utils/utils/vector_set.h"

namespace sge {

struct MDiffuseMaterial;

//-------------------------------------------------------------------
// TraitModel
//-------------------------------------------------------------------
// clang-format off
DefineTypeId(TraitModel, 20'03'01'0004);
DefineTypeId(TraitModel::MaterialOverride, 20'10'15'0001);
DefineTypeId(std::vector<TraitModel::MaterialOverride>, 20'10'15'0002);
DefineTypeId(TraitModel::ImageSettings, 21'04'04'0001);

ReflBlock() {

	ReflAddType(TraitModel::ImageSettings)
		ReflMember(TraitModel::ImageSettings, m_anchor)
		ReflMember(TraitModel::ImageSettings, defaultFacingAxisZ)
		ReflMember(TraitModel::ImageSettings, m_localXOffset).uiRange(-FLT_MAX, FLT_MAX, 0.01f)
		ReflMember(TraitModel::ImageSettings, m_pixelsPerUnit).uiRange(0.00001f, 100000.f, 0.1f)
		ReflMember(TraitModel::ImageSettings, m_billboarding)
		ReflMember(TraitModel::ImageSettings, forceNoLighting)
		ReflMember(TraitModel::ImageSettings, forceNoCulling)
		ReflMember(TraitModel::ImageSettings, spriteFrameTime).uiRange(0.f, 100000.f, 0.01f);
	;

	ReflAddType(TraitModel::MaterialOverride)
		ReflMember(TraitModel::MaterialOverride, materialName)
		ReflMember(TraitModel::MaterialOverride, materialObjId).setPrettyName("Material Object")
	;

	ReflAddType(std::vector<TraitModel::MaterialOverride>);

	ReflAddType(TraitModel)
		ReflMember(TraitModel, m_assetProperty)
		ReflMember(TraitModel, m_materialOverrides)
		ReflMember(TraitModel, useSkeleton)
		ReflMember(TraitModel, rootSkeletonId)
		ReflMember(TraitModel, imageSettings)
	;

}
// clang-format on

void TraitModel::computeNodeToBoneIds() {
	if (nodeToBoneId.empty() == false) {
		return;
	}

	nodeToBoneId.clear();

	if (useSkeleton == false) {
		return;
	}

	AssetModel* assetmodel = m_assetProperty.getAssetModel();
	if (assetmodel == nullptr) {
		return;
	}

	GameWorld* world = getWorldFromObject();
	vector_set<ObjectId> boneActorIds;
	world->getAllChildren(boneActorIds, rootSkeletonId);
	boneActorIds.insert(rootSkeletonId);

	vector_set<Actor*> boneActors;
	for (ObjectId boneId : boneActorIds) {
		Actor* bone = world->getActorById(boneId);
		if (bone == nullptr) {
			nodeToBoneId.clear();
			sgeAssert("Could not find a bone for the actor!");
			return;
		}

		boneActors.insert(bone);
	}

	for (const Model::Node* modelNode : assetmodel->model.m_nodes) {
		for (Actor* boneActor : boneActors) {
			if (modelNode->name == boneActor->getDisplayName()) {
				nodeToBoneId[modelNode] = boneActor->getId();
				break;
			}
		}
	}
}

void TraitModel::computeSkeleton(vector_map<const Model::Node*, mat4f>& boneOverrides) {
	boneOverrides.clear();

	if (useSkeleton == false) {
		return;
	}

	computeNodeToBoneIds();
	Actor* root = getWorldFromObject()->getActorById(rootSkeletonId);
	if (root == nullptr) {
		return;
	}

	mat4f rootInv = root ? root->getTransformMtx().inverse() : mat4f::getIdentity();

	for (auto pair : nodeToBoneId) {
		mat4f& boneGobalTForm = boneOverrides[pair.first];

		Actor* const boneActor = getWorldFromObject()->getActorById(pair.second);
		if (boneActor) {
			boneGobalTForm = rootInv * boneActor->getTransformMtx();
		} else {
			sgeAssert(false && "Expected alive object");
			boneGobalTForm = mat4f::getIdentity();
		}
	}
}


void editTraitStaticModel(GameInspector& inspector, GameObject* actor, MemberChain chain) {
	TraitModel& traitStaticModel = *(TraitModel*)chain.follow(actor);

	ImGuiEx::BeginGroupPanel("Static Model");

	chain.add(sgeFindMember(TraitModel, m_assetProperty));
	ProperyEditorUIGen::doMemberUI(inspector, actor, chain);
	chain.pop();

	if (AssetModel* loadedAsset = traitStaticModel.m_assetProperty.getAssetModel()) {
		for (int t = 0; t < loadedAsset->model.m_materials.size(); ++t) {
			// ImGui::Text(loadedAsset->model.m_materials[t]->name.c_str());
			// Check if override for this material already exists.
			auto itrExisting = std::find_if(traitStaticModel.m_materialOverrides.begin(), traitStaticModel.m_materialOverrides.end(),
			                                [&](const TraitModel::MaterialOverride& ovr) -> bool {
				                                return ovr.materialName == loadedAsset->model.m_materials[t]->name;
			                                });

			if (itrExisting == std::end(traitStaticModel.m_materialOverrides)) {
				TraitModel::MaterialOverride ovr;
				ovr.materialName = loadedAsset->model.m_materials[t]->name;
				traitStaticModel.m_materialOverrides.emplace_back(ovr);
			}
		}

		// Now do the UI for each available slot (keep in mind that we are keeping old slots (from previous models still available).
		{
			for (int t = 0; t < loadedAsset->model.m_materials.size(); ++t) {
				ImGui::PushID(t);

				// ImGui::Text(loadedAsset->model.m_materials[t]->name.c_str());
				// Check if override for this material already exists.
				auto itrExisting = std::find_if(traitStaticModel.m_materialOverrides.begin(), traitStaticModel.m_materialOverrides.end(),
				                                [&](const TraitModel::MaterialOverride& ovr) -> bool {
					                                return ovr.materialName == loadedAsset->model.m_materials[t]->name;
				                                });

				if (itrExisting != std::end(traitStaticModel.m_materialOverrides)) {
					ImGuiEx::BeginGroupPanel(itrExisting->materialName.c_str(), ImVec2(-1.f, 0.f));
					{
						const int slotIndex = int(itrExisting - traitStaticModel.m_materialOverrides.begin());
						chain.add(sgeFindMember(TraitModel, m_materialOverrides), slotIndex);
						chain.add(sgeFindMember(TraitModel::MaterialOverride, materialObjId));
						ProperyEditorUIGen::doMemberUI(inspector, actor, chain);

						if (traitStaticModel.m_materialOverrides[slotIndex].materialObjId.isNull()) {
							if (ImGui::Button("Create New Material")) {
								CmdObjectCreation* cmdMtlCreate = new CmdObjectCreation();
								cmdMtlCreate->setup(sgeTypeId(MDiffuseMaterial));
								cmdMtlCreate->apply(&inspector);

								CmdMemberChange* const cmdAssignMaterial = new CmdMemberChange();
								const ObjectId originalValue = ObjectId();
								const ObjectId createdMaterialId = cmdMtlCreate->getCreatedObjectId();
								cmdAssignMaterial->setup(actor, chain, &originalValue, &createdMaterialId, nullptr);
								cmdAssignMaterial->apply(&inspector);

								CmdCompound* cmdCompound = new CmdCompound();
								cmdCompound->addCommand(cmdMtlCreate);
								cmdCompound->addCommand(cmdAssignMaterial);
								inspector.appendCommand(cmdCompound, false);
							}
						}

						chain.pop();
						chain.pop();

						if (GameObject* const materialObject = inspector.getWorld()->getObjectById(itrExisting->materialObjId)) {
							ImGui::PushID(materialObject);
							ImGuiEx::BeginGroupPanel("", ImVec2(-1.f, 0.f));
							if (ImGui::CollapsingHeader("Material")) {
								ProperyEditorUIGen::doGameObjectUI(inspector, materialObject);
							}
							ImGuiEx::EndGroupPanel();
							ImGui::PopID();
						}
					}
					ImGuiEx::EndGroupPanel();
				} else {
					sgeAssert(false && "The slot should have been created above.");
				}

				ImGui::PopID();
			}
		}


		if (ImGui::Button("Extract Skeleton")) {
			AssetModel* modelAsset = traitStaticModel.m_assetProperty.getAssetModel();
			GameWorld* world = inspector.getWorld();
			ALocator* allBonesParent = world->alloc<ALocator>();
			allBonesParent->setTransform(traitStaticModel.getActor()->getTransform());

			float boneLengthAuto = 0.05f * modelAsset->staticEval.aabox.diagonal().length();

			struct NodeRemapEl {
				transf3d localTransform;
				ABone* boneActor = nullptr;
			};
			vector_map<Model::Node*, NodeRemapEl> nodeRemap;

			transf3d n2w = actor->getActor()->getTransform();
			for (Model::Node* node : modelAsset->model.m_nodes) {
				ParameterBlock& pb = node->paramBlock;
				const Parameter* const scalingPrm = pb.FindParameter("scaling");
				const Parameter* const rotationPrm = pb.FindParameter("rotation");
				const Parameter* const translationPrm = pb.FindParameter("translation");

				const Parameter* const boneLengthPrm = pb.FindParameter("boneLength");

				transf3d localTform;

				scalingPrm->Evalute(&localTform.s, nullptr, 0.f);
				rotationPrm->Evalute(&localTform.r, nullptr, 0.f);
				translationPrm->Evalute(&localTform.p, nullptr, 0.f);

				float boneLength = boneLengthAuto;
				if (boneLengthPrm) {
					boneLengthPrm->Evalute(&boneLength, nullptr, 0.f);
				}

				nodeRemap[node].localTransform = localTform;
				nodeRemap[node].boneActor = inspector.getWorld()->alloc<ABone>(ObjectId(), node->name.c_str());
				nodeRemap[node].boneActor->boneLength = boneLength;
			}

			std::function<void(Model::Node * node, NodeRemapEl * parent)> traverseGlobalTransform;
			traverseGlobalTransform = [&](Model::Node* node, NodeRemapEl* parent) -> void {
				NodeRemapEl* remapNode = nodeRemap.find_element(node);
				if (parent) {
					remapNode->boneActor->setTransform(parent->boneActor->getTransform() * remapNode->localTransform);
					inspector.getWorld()->setParentOf(remapNode->boneActor->getId(), parent->boneActor->getId());
				} else {
					remapNode->localTransform = allBonesParent->getTransform() * remapNode->localTransform;
					remapNode->boneActor->setTransform(remapNode->localTransform);
					world->setParentOf(remapNode->boneActor->getId(), allBonesParent->getId());
				}

				for (auto& childNode : node->childNodes) {
					traverseGlobalTransform(childNode, remapNode);
				}
			};


			traverseGlobalTransform(modelAsset->model.m_rootNode, nullptr);
		}

		chain.add(sgeFindMember(TraitModel, useSkeleton));
		ProperyEditorUIGen::doMemberUI(inspector, actor, chain);
		chain.pop();

		chain.add(sgeFindMember(TraitModel, rootSkeletonId));
		ProperyEditorUIGen::doMemberUI(inspector, actor, chain);
		chain.pop();
	}

	if (traitStaticModel.m_assetProperty.getAssetSprite()) {
		chain.add(sgeFindMember(TraitModel, imageSettings));
		ProperyEditorUIGen::doMemberUI(inspector, actor, chain);
		chain.pop();
	}

	if (traitStaticModel.m_assetProperty.getAssetTexture()) {
		chain.add(sgeFindMember(TraitModel, imageSettings));
		ProperyEditorUIGen::doMemberUI(inspector, actor, chain);
		chain.pop();
	}


	ImGuiEx::EndGroupPanel();
}

SgePluginOnLoad() {
	getEngineGlobal()->addPropertyEditorIUGeneratorForType(sgeTypeId(TraitModel), editTraitStaticModel);
}

} // namespace sge
