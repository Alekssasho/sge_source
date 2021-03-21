#pragma once

#include <fbxsdk.h>

#include "ModelParseSettings.h"
#include "sge_utils/math/transform.h"
#include "sge_core/model/Model.h"

namespace sge {

struct IAssetRelocationPolicy;

bool InitializeFBXSDK();

//---------------------------------------------------------------
//
//---------------------------------------------------------------
struct FBXSDKParser {
	// Prases the input FBX scene and produces a ready to save SGE model.
	// @param enforcedRootNode is the node to be used as a root node insted of the actual one, if null, the regular root is going to be
	// used.
	bool parse(Model::Model* result,
	           std::vector<std::string>* pReferencedTextures,
	           fbxsdk::FbxScene* scene,
	           FbxNode* enforcedRootNode,
	           const ModelParseSettings& parseSettings);

  private:
	// Step 1: parse the materials.
	void parseMaterials();

	// Step 2: parse the meshes.
	void parseMeshes();

	// Step 3: parse the node hierarchy.
	Model::Node* parseNodesRecursive(fbxsdk::FbxNode* const fbxNode, const fbxsdk::FbxAMatrix* const pOverrideTransform = nullptr);

	// Step 4: resolve bones to nodes pointer.
	void resolveBonesNodePointer();

	// Step 5: parse the animation.
	void parseAnimations();

	// Step 6: parse collision geometry.
	void parseCollisionGeometry();

	// Mesh parsing helpers.
	void parseMesh(fbxsdk::FbxMesh* const fbxMesh);
	Model::MeshData* findBestSuitableMeshData(fbxsdk::FbxMesh* const fbxMesh);

	int getNextId() {
		return m_nextFreeId++;
	}

	int m_nextFreeId = 0;

	Model::Model* m_model = nullptr;
	fbxsdk::FbxScene* m_fbxScene = nullptr;
	ModelParseSettings m_parseSettings;
	std::vector<std::string>* m_pReferencedTextures = nullptr;

	vector_map<fbxsdk::FbxSurfaceMaterial*, Model::Material*> fbxSurfMtlToMtl;
	vector_map<fbxsdk::FbxTexture*, Model::Material*> fbxTexDuffuseToMtl; // Diffuse texture used in material.
	vector_map<fbxsdk::FbxNode*, Model::Node*> fbxNodeToNode;
	std::map<Model::Bone*, fbxsdk::FbxNode*> bonesToResolve;

	// Geometry to be parsed as collision geometry.
	transf3d m_collision_transfromCorrection =
	    transf3d::getIdentity(); // When enforcing a root node we need to remove it's transfrom from the collsion objects.
	std::map<FbxMesh*, std::vector<transf3d>> m_collision_ConvexHullMeshes;
	std::map<FbxMesh*, std::vector<transf3d>> m_collision_BvhTriMeshes;
	std::map<FbxMesh*, std::vector<transf3d>> m_collision_BoxMeshes;
	std::map<FbxMesh*, std::vector<transf3d>> m_collision_CaplsuleMeshes;
	std::map<FbxMesh*, std::vector<transf3d>> m_collision_CylinderMeshes;
	std::map<FbxMesh*, std::vector<transf3d>> m_collision_SphereMeshes;

	// Converts an FBX SDK imported mesh to a mesh.
	// Note that we are counting on having each FBXMesh split per material, otherwise this would not be possible.
	// FBXMesh != Model::Mesh otherwise.
	std::map<fbxsdk::FbxMesh*, Model::Mesh*> fbxMeshToMesh;

	// Unlike assimp in FBX a single mesh could have multiple materials attached to a single mesh.
	std::map<fbxsdk::FbxMesh*, std::vector<Model::Mesh>> fbxMeshToMeshes;
};

} // namespace sge
