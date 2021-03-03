#include <filesystem>

#include "AssetsWindow.h"
#include "IAssetRelocationPolicy.h"
#include "IconsForkAwesome/IconsForkAwesome.h"
#include "ModelParseSettings.h"
#include "sge_core/AssetLibrary.h"
#include "sge_core/ICore.h"
#include "sge_core/SGEImGui.h"
#include "sge_core/model/ModelReader.h"
#include "sge_core/model/ModelWriter.h"
#include "sge_engine/EngineGlobal.h"
#include "sge_engine/GameInspector.h"
#include "sge_engine/GameWorld.h"
#include "sge_utils/tiny/FileOpenDialog.h"
#include "sge_utils/utils/Path.h"
#include "sge_utils/utils/strings.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINAMX
#include <Shlobj.h>
#include <Windows.h>
#include <shellapi.h>
#endif

namespace sge {

void AssetsWindow::openAssetImport(const std::string& filename) {
	AssetLibrary* const assetLib = getCore()->getAssetLib();

	AssetImportData aid;
	aid.filename = filename;
	aid.assetType = assetType_fromExtension(extractFileExtension(filename.c_str()).c_str());
	if (aid.assetType != AssetType::None) {
		if (aid.assetType == AssetType::Model) {
			Model::Model importedModel;
			ModelParseSettings mps;
			NoneAssetRelocationPolicy relocationPolicy = NoneAssetRelocationPolicy();
			mps.pRelocaionPolicy = &relocationPolicy;

			std::vector<std::string> referencedTextures;

			if (sgeImportFBXFile(importedModel, aid.filename.c_str(), mps, &referencedTextures)) {
				createDirectory(extractFileDir(aid.outputFilename.c_str(), false).c_str());
				ModelWriter modelWriter;

				WriteByteStream serializedModel;
				const bool succeeded = modelWriter.write(importedModel, &serializedModel);

				ReadByteStream serializedModelReader(serializedModel.serializedData);
				Model::ModelReader modelReader;

				// Create the temporary asset.
				aid.tempAsset = assetLib->makeRuntimeAsset(AssetType::Model, ("@tmp_" + filename).c_str());

				Model::LoadSettings loadSets;
				loadSets.assetDir = extractFileDir(aid.filename.c_str(), true);
				modelReader.Load(loadSets, getCore()->getDevice(), &serializedModelReader, aid.tempAsset->asModel()->model);

				aid.tempAsset->asModel()->staticEval.initialize(assetLib, &aid.tempAsset->asModel()->model);
				aid.tempAsset->asModel()->staticEval.evaluate(nullptr, 0);

				aid.tempAsset->asModel()->sharedEval.initialize(assetLib, &aid.tempAsset->asModel()->model);
				aid.tempAsset->asModel()->sharedEval.evaluate(nullptr, 0);
			}
		}

		m_assetsToImport.emplace_back(std::move(aid));
	} else {
		std::string msg = string_format("Unknown asset type for %s", filename.c_str());
		getEngineGlobal()->showNotification(msg.c_str());
		SGE_DEBUG_ERR(msg.c_str());
	}
}

bool AssetsWindow::importAsset(AssetImportData& aid) {
	AssetLibrary* const assetLib = getCore()->getAssetLib();

	std::string fullAssetPath = aid.outputDir + "/" + aid.outputFilename;

	if (aid.assetType == AssetType::Model) {
		Model::Model importedModel;
		NoneAssetRelocationPolicy relocationPolicy = NoneAssetRelocationPolicy();
		ModelParseSettings mps;
		mps.pRelocaionPolicy = &relocationPolicy;
		std::vector<std::string> referencedTextures;
		if (sgeImportFBXFile(importedModel, aid.filename.c_str(), mps, &referencedTextures)) {
			createDirectory(extractFileDir(aid.outputDir.c_str(), false).c_str());

			// Convert the 3d model to our internal type.
			ModelWriter modelWriter;
			const bool succeeded = modelWriter.write(importedModel, fullAssetPath.c_str());

			PAsset assetModel = assetLib->getAsset(AssetType::Model, fullAssetPath.c_str(), true);
			assetLib->reloadAssetModified(assetModel.get());

			std::string notificationMsg = string_format("Imported %s", fullAssetPath.c_str());
			SGE_DEBUG_LOG(notificationMsg.c_str());
			getEngineGlobal()->showNotification(notificationMsg);

			// Copy the referenced textures.
			const std::string modelInputDir = extractFileDir(aid.filename.c_str(), true);
			for (const std::string& texturePathLocal : referencedTextures) {
				const std::string textureDestDir = aid.outputDir + "/" + extractFileDir(texturePathLocal.c_str(), true);
				const std::string textureFilename = extractFileNameIncludingExtension(texturePathLocal.c_str());

				const std::string textureSrcPath = canonizePathRespectOS(modelInputDir + texturePathLocal);
				const std::string textureDstPath = canonizePathRespectOS(textureDestDir + textureFilename);

				createDirectory(textureDestDir.c_str());
				copyFile(textureSrcPath.c_str(), textureDstPath.c_str());

				PAsset assetTexture = assetLib->getAsset(AssetType::TextureView, textureDstPath.c_str(), true);
				assetLib->reloadAssetModified(assetTexture.get());
			}

			return true;
		} else {
			std::string notificationMsg = string_format("Failed to import %s", fullAssetPath.c_str());
			SGE_DEBUG_ERR(notificationMsg.c_str());
			getEngineGlobal()->showNotification(notificationMsg);

			return false;
		}
	} else if (aid.assetType == AssetType::TextureView) {
		// TODO: DDS conversion.
		createDirectory(extractFileDir(aid.outputDir.c_str(), false).c_str());
		copyFile(aid.filename.c_str(), fullAssetPath.c_str());

		PAsset assetTexture = assetLib->getAsset(AssetType::TextureView, fullAssetPath.c_str(), true);
		assetLib->reloadAssetModified(assetTexture.get());

		return true;
	} else if (aid.assetType == AssetType::Text) {
		createDirectory(extractFileDir(aid.outputDir.c_str(), false).c_str());
		copyFile(aid.filename.c_str(), fullAssetPath.c_str());

		PAsset asset = assetLib->getAsset(AssetType::Text, fullAssetPath.c_str(), true);
		assetLib->reloadAssetModified(asset.get());

		return true;
	}

	sgeAssertFalse("unknown asset type");
	return false;
}

void AssetsWindow::update_assetImport(SGEContext* const sgecon, const InputState& is) {
	if (ImGui::Button(ICON_FK_PLUS " Add Asset")) {
		std::string filename = FileOpenDialog("Import 3D Model", true, "*.fbx\0*.fbx\0*.dae\0*.dae\0*.obj\0*.obj\0*.*\0*.*\0");
		if (filename.empty() == false) {
			openAssetImport(filename);
		}
	}

	if (ImGui::BeginChild("Child Assets To Import")) {
		std::string groupPanelName;
		for (int iAsset = 0; iAsset < m_assetsToImport.size(); ++iAsset) {
			AssetImportData& aid = m_assetsToImport[iAsset];
			ImGui::PushID(&aid);

			if (aid.importFailed) {
				ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Failed to Import");
			}

			string_format(groupPanelName, "3D Model %s", aid.filename.c_str());
			ImGuiEx::BeginGroupPanel(groupPanelName.c_str());
			{
				if (ImGui::Button("Import As")) {
					aid.outputFilename = FileSaveDialog("Import 3D Model As", "*.mdl\0*.mdl", "mdl");
				}
				ImGui::SameLine();
				char importAs[1024] = {0};
				strcpy_s(importAs, aid.outputFilename.c_str());
				if (ImGui::InputText("##Import As", importAs, SGE_ARRSZ(importAs))) {
					aid.outputFilename = importAs;
				}

				ImGui::Checkbox("Preview", &aid.preview);
				if (aid.preview) {
					if (aid.assetType == AssetType::Model) {
						aid.modelPreviewWidget.doWidget(sgecon, is, aid.tempAsset->asModel()->staticEval, vec2f(-1.f, 256.f));
					}
				}

				if (ImGui::Button(ICON_FK_DOWNLOAD "Import")) {
					if (importAsset(aid)) {
						m_assetsToImport.erase(m_assetsToImport.begin() + iAsset);
						iAsset--;
					}
				}

				ImGui::SameLine();
				if (ImGui::Button(ICON_FK_TRASH " Remove")) {
					m_assetsToImport.erase(m_assetsToImport.begin() + iAsset);
					iAsset--;
				}
			}
			ImGuiEx::EndGroupPanel();

			ImGui::PopID();
		}

		if (ImGui::Button("Import All")) {
		}

		ImGui::EndChild();
	}
}

void AssetsWindow::update(SGEContext* const sgecon, const InputState& is) {
	if (isClosed()) {
		return;
	}

	AssetLibrary* const assetLib = getCore()->getAssetLib();

	if (ImGui::Begin(m_windowName.c_str(), &m_isOpened)) {
		if (ImGui::BeginTabBar("Assets Tab Bar")) {
			if (ImGui::BeginTabItem(ICON_FK_SEARCH " Explore")) {
				namespace fs = std::filesystem;

				if (ImGui::Button(ICON_FK_BACKWARD)) {
					if (directoryTree.empty() == false) {
						directoryTree.pop_back();
					}
				}

				ImGui::SameLine();
				ImGui::Separator();
				ImGui::SameLine();
				ImGui::Text("Path: ");

				{
					int eraseAfter = -1;
					for (int t = 0; t < int(directoryTree.size()); ++t) {
						ImGui::SameLine();

						if (ImGui::Button(directoryTree[t].c_str())) {
							eraseAfter = t + 1;
						}
					}

					if (eraseAfter > -1) {
						while (directoryTree.size() > eraseAfter) {
							directoryTree.pop_back();
						}
					}
				}

				ImGui::NewLine();

				bool explorePreviewAssetChanged = false;
				if (explorePreviewAsset) {
					ImGui::Columns(2);
				}

				ImGui::Text(ICON_FK_SEARCH " File Filter");
				ImGui::SameLine();
				exploreFilter.Draw("##File Filter");
				if (ImGui::IsItemClicked(2)) {
					ImGui::ClearActiveID(); // Hack: (if we do not make this call ImGui::InputText will set it's cached value.
					exploreFilter.Clear();
				}

				if (ImGui::BeginChildFrame(55322121, ImVec2(-1.f, -1.f))) {
					try {
						if (!directoryTree.empty() && ImGui::Selectable(ICON_FK_BACKWARD " [/..]")) {
							directoryTree.pop_back();
						}

						bool shouldOpenNewFolderPopup = false;

						std::string label;
						fs::path pathToAssets = assetLib->getAssetsDirAbs();
						for (auto& p : directoryTree) {
							pathToAssets.append(p);
						}

						if (ImGui::Selectable(ICON_FK_FOLDER_O " [New Folder]")) {
							shouldOpenNewFolderPopup = true;
						}

						Optional<fs::path> rightClickedPath;

						std::string dirToAdd;
						for (const fs::directory_entry& entry : fs::directory_iterator(pathToAssets)) {
							if (entry.is_directory() && exploreFilter.PassFilter(entry.path().filename().string().c_str())) {
								string_format(label, "%s %s", ICON_FK_FOLDER, entry.path().filename().string().c_str());
								if (ImGui::Selectable(label.c_str())) {
									dirToAdd = entry.path().filename().string();
								}

								if (ImGui::IsItemClicked(1)) {
									rightClickedPath = entry.path();
								}
							}
						}

						for (const fs::directory_entry& entry : fs::directory_iterator(pathToAssets)) {
							if (entry.is_regular_file() && exploreFilter.PassFilter(entry.path().filename().string().c_str())) {
								AssetType assetType = assetType_fromExtension(extractFileExtension(entry.path().string().c_str()).c_str());
								if (assetType == AssetType::Model) {
									string_format(label, "%s %s", ICON_FK_CUBE, entry.path().filename().string().c_str());
								} else if (assetType == AssetType::TextureView) {
									string_format(label, "%s %s", ICON_FK_PICTURE_O, entry.path().filename().string().c_str());
								} else if (assetType == AssetType::Text) {
									string_format(label, "%s %s", ICON_FK_FILE, entry.path().filename().string().c_str());
								} else {
									string_format(label, "%s %s", ICON_FK_FILE_TEXT_O, entry.path().filename().string().c_str());
								}

								if (ImGui::Selectable(label.c_str())) {
									explorePreviewAssetChanged = true;
									explorePreviewAsset = assetLib->getAsset(entry.path().string().c_str(), true);
								}
								if (ImGui::IsItemClicked(1)) {
									rightClickedPath = entry.path();
								}
							}
						}

						if (rightClickedPath.hasValue()) {
							ImGui::OpenPopup("RightClickMenuAssets");
							m_rightClickedPath = rightClickedPath.get();
						} else if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(1)) {
							ImGui::OpenPopup("RightClickMenuAssets");
							m_rightClickedPath.clear();
						}

						// Right click menu.
						bool shouldOpenImportPopup = false;
						fs::path importOverAsset;
						if (ImGui::BeginPopup("RightClickMenuAssets")) {
							if (ImGui::MenuItem(ICON_FK_DOWNLOAD " Import here...")) {
								shouldOpenImportPopup = true;
							}

							if (ImGui::MenuItem(ICON_FK_FOLDER " New Folder")) {
								shouldOpenNewFolderPopup = true;
							}

							if (!m_rightClickedPath.empty()) {
								ImGui::Separator();

								if (ImGui::MenuItem(ICON_FK_CLIPBOARD " Copy Path")) {
									ImGui::SetClipboardText(m_rightClickedPath.string().c_str());
								}

								if (ImGui::MenuItem(ICON_FK_REFRESH " Import Over")) {
									importOverAsset = m_rightClickedPath;
									shouldOpenImportPopup = true;
								}

#ifdef WIN32
								if (ImGui::MenuItem(ICON_FK_WINDOWS " Open in Explorer")) {
									PIDLIST_ABSOLUTE pItem = ILCreateFromPathA(m_rightClickedPath.string().c_str());
									if (pItem) {
										SHOpenFolderAndSelectItems(pItem, 0, NULL, 0);
										ILFree(pItem);
									}
								}
							}
#endif

							ImGui::EndPopup();
						}

						// Import Popup
						if (shouldOpenImportPopup) {
							ImGui::OpenPopup("SGE Assets Window Import Popup");
						}

						if (ImGui::BeginPopup("SGE Assets Window Import Popup")) {
							if (shouldOpenImportPopup) {
								// If this is still true then the popup has just been opened.
								// Initialize it with the information about the asset we are about to import.
								m_importAssetToImportInPopup = AssetImportData();
								m_importAssetToImportInPopup.filename = FileOpenDialog("Pick a file to import", true, "*.*\0*.*\0");
								m_importAssetToImportInPopup.assetType =
								    assetType_fromExtension(extractFileExtension(m_importAssetToImportInPopup.filename.c_str()).c_str());

								m_importAssetToImportInPopup.outputDir = pathToAssets.string();
								if (importOverAsset.empty()) {
									m_importAssetToImportInPopup.outputFilename =
									    extractFileNameIncludingExtension(m_importAssetToImportInPopup.filename.c_str());

								} else {
									m_importAssetToImportInPopup.outputFilename = importOverAsset.filename().string();
									importOverAsset.clear();
								}
								if (m_importAssetToImportInPopup.assetType == AssetType::Model) {
									m_importAssetToImportInPopup.outputFilename =
									    replaceExtension(m_importAssetToImportInPopup.outputFilename.c_str(), "mdl");
								}

								if (m_importAssetToImportInPopup.filename.empty()) {
									ImGui::CloseCurrentPopup();
								}
							}

							// The UI
							if (m_importAssetToImportInPopup.assetType == AssetType::Model) {
								ImGui::Text(ICON_FK_CUBE " 3D Model");
							} else if (m_importAssetToImportInPopup.assetType == AssetType::TextureView) {
								ImGui::Text(ICON_FK_PICTURE_O " Texture");
							} else if (m_importAssetToImportInPopup.assetType == AssetType::Text) {
								ImGui::Text(ICON_FK_FILE " Text");
							} else {
								ImGui::Text(ICON_FK_FILE_TEXT_O "Unknown");
							}

							ImGuiEx::InputText("Read From", m_importAssetToImportInPopup.filename, ImGuiInputTextFlags_ReadOnly);
							ImGuiEx::InputText("Import As", m_importAssetToImportInPopup.filename);

							if (ImGui::Button(ICON_FK_DOWNLOAD " Import")) {
								importAsset(m_importAssetToImportInPopup);
								ImGui::CloseCurrentPopup();
							}
							ImGui::SameLine();
							if (ImGui::Button(ICON_FK_DOWNLOAD " Cancel")) {
								ImGui::CloseCurrentPopup();
							}

							ImGui::EndPopup();
						}

						// Create Directory Popup
						if (shouldOpenNewFolderPopup) {
							ImGui::OpenPopup("SGE Assets Window Create Dir");
						}

						static char createDirFileName[1024] = {0};
						if (ImGui::BeginPopup("SGE Assets Window Create Dir")) {
							ImGui::InputText(ICON_FK_FOLDER " Folder Name", createDirFileName, SGE_ARRSZ(createDirFileName));
							if (ImGui::Button(ICON_FK_CHECK " Create")) {
								createDirectory((pathToAssets.string() + "/" + std::string(createDirFileName)).c_str());
								createDirFileName[0] = '\0';
								ImGui::CloseCurrentPopup();
							}

							if (ImGui::Button("Cancel")) {
								ImGui::CloseCurrentPopup();
							}

							ImGui::EndPopup();
						}

						if (dirToAdd.empty() == false) {
							directoryTree.emplace_back(std::move(dirToAdd));
						}
					}


					catch (...) {
					}
				}
				ImGui::EndChildFrame();

				if (isAssetLoaded(explorePreviewAsset)) {
					ImGui::NextColumn();

					if (explorePreviewAsset->getType() == AssetType::Model) {
						if (explorePreviewAssetChanged) {
							AABox3f bboxModel = explorePreviewAsset->asModel()->staticEval.aabox;
							if (bboxModel.IsEmpty() == false) {
								exploreModelPreviewWidget.camera.orbitPoint = bboxModel.center();
								exploreModelPreviewWidget.camera.radius = bboxModel.diagonal().length() * 1.25f;
								exploreModelPreviewWidget.camera.yaw = deg2rad(45.f);
								exploreModelPreviewWidget.camera.pitch = deg2rad(45.f);
							}
						}

						exploreModelPreviewWidget.doWidget(sgecon, is, explorePreviewAsset->asModel()->staticEval);
					} else if (explorePreviewAsset->getType() == AssetType::TextureView) {
						auto desc = explorePreviewAsset->asTextureView()->GetPtr()->getDesc().texture2D;
						ImVec2 sz = ImGui::GetContentRegionAvail();
						ImGui::Image(explorePreviewAsset->asTextureView()->GetPtr(), sz);
					} else {
						ImGui::Text("No Preview");
					}
				}


				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem(ICON_FK_DOWNLOAD " Import")) {
				update_assetImport(sgecon, is);
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}

} // namespace sge
