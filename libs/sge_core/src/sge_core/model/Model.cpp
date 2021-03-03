#include <functional>

#include "Model.h"

namespace sge {

namespace Model {

	// CollisionMesh CollisionMesh::getCombiendMesh(const std::vector<CollisionMesh>& collisionMeshes) {
	//	CollisionMesh result;
	//	int indexOffset = 0;
	//	for (const CollisionMesh& meshToAppend : collisionMeshes) {
	//		result.vertices.emplace_back(meshToAppend.vertices);

	//		sgeAssert(meshToAppend.indices.size() % 3 == 0);
	//		result.indices.reserve(result.indices.size() + meshToAppend.indices.size());
	//		for (const int index : meshToAppend.indices) {
	//			result.indices.push_back(index + indexOffset);
	//		}

	//		indexOffset += int(meshToAppend.indices.size());
	//	}
	//}

	Material* Model::FindMaterial(const int id) {
		for (Material* mtl : m_materials) {
			if (mtl->id == id) {
				return mtl;
			}
		}

		return NULL;
	}

	Node* Model::FindNode(const int id) {
		for (Node* node : m_nodes) {
			if (node->id == id) {
				return node;
			}
		}

		return NULL;
	}

	const Node* Model::FindNode(const int id) const {
		for (Node* node : m_nodes) {
			if (node->id == id) {
				return node;
			}
		}

		return NULL;
	}

	const Node* Model::FindFirstNodeByName(const std::string& name) const {
		for (Node* node : m_nodes) {
			if (node->name == name) {
				return node;
			}
		}

		return NULL;
	}

	Mesh* Model::FindMesh(const int id) {
		for (MeshData* meshData : m_meshesData) {
			for (auto& mesh : meshData->meshes) {
				if (mesh->id == id) {
					return mesh;
				}
			}
		}

		return NULL;
	}

	const AnimationInfo* Model::findAnimation(const std::string& name) const {
		for (auto& anim : m_animations) {
			if (anim.curveName == name)
				return &anim;
		}

		return nullptr;
	}

	float Mesh::Raycast(const Ray& ray, const char* positionSemantic) const {
		if (pMeshData == nullptr || pMeshData->vertexBufferRaw.size() == 0 || primTopo != PrimitiveTopology::TriangleList ||
		    positionSemantic == nullptr) {
			//(
			//    "%s: Failed! There isn't avaiable mesh data"
			//    "OR the primitive toplogy is != TriangleList! Mesh: '%s'!\n",
			//    __FUNCTION__, name.c_str());
			sgeAssert(false);
			return false;
		}

		sgeAssert(primTopo == PrimitiveTopology::TriangleList);

		// Position semantic byte offset.
		int posByteOffset = -1;
		for (const auto& decl : vertexDecl) {
			if (decl.semantic == positionSemantic) {
				posByteOffset = (int)decl.byteOffset;
				break;
			}
		}

		if (posByteOffset < 0) {
			sgeAssert(false && "Failed! Position semantic is missing! ");
			return false;
		}

		// Check if there is a index buffer.
		if (ibFmt != UniformType::Unknown) {
			if (pMeshData->indexBufferRaw.size() == 0)
				return false;

			if (ibFmt == UniformType::Uint16)
				return RaycastIndexBuffer<uint16>(ray, posByteOffset);
			if (ibFmt == UniformType::Uint)
				return RaycastIndexBuffer<uint32>(ray, posByteOffset);
			else {
				sgeAssert(false && "Failed unsupported index buffer format!");
				return false;
			}
		}

		// No index buffer, raycast vs vertex buffer.
		return RaycastVertexBufferOnly(ray, posByteOffset);
	}

	template <typename T>
	float Mesh::RaycastIndexBuffer(const Ray& ray, const int posByteOffset, std::vector<float>* pAllIntersections) const {
		const char* const vb = (char*)pMeshData->vertexBufferRaw.data();
		const T* const ib = (T*)pMeshData->indexBufferRaw.data();
		const size_t numIndices = pMeshData->indexBufferRaw.size() / sizeof(T);

		float mint = FLT_MAX; // Something invalid.
		for (size_t iIdx = 0; iIdx < numIndices; iIdx += 3) {
			const int vertByteOffset[3] = {
			    int(ib[iIdx] * stride) + posByteOffset,
			    int(ib[iIdx + 1] * stride) + posByteOffset,
			    int(ib[iIdx + 2] * stride) + posByteOffset,
			};

			const vec3f p[] = {
			    *(vec3f*)(vb + vertByteOffset[0]),
			    *(vec3f*)(vb + vertByteOffset[1]),
			    *(vec3f*)(vb + vertByteOffset[2]),
			};

			const float t = IntersectRayTriangle(ray, p);
			if (t < mint) {
				mint = t;
			}

			if (pAllIntersections != nullptr && t != FLT_MAX) {
				pAllIntersections->push_back(t);
			}
		}

		return mint;
	}

	float Mesh::RaycastVertexBufferOnly(const Ray& ray, const int posByteOffset, std::vector<float>* pAllIntersections) const {
		const char* const vb = (char*)pMeshData->vertexBufferRaw.data();
		// const size_t numVertices = pMeshData->vertexBufferRaw.size() / stride;

		float mint = FLT_MAX; // Something invalid.
		for (int iVtx = 0; iVtx < numVertices; iVtx += 3) {
			const vec3f p[3] = {
			    *(vec3f*)(vb + (iVtx)*stride + posByteOffset),
			    *(vec3f*)(vb + (iVtx + 1) * stride + posByteOffset),
			    *(vec3f*)(vb + (iVtx + 2) * stride + posByteOffset),
			};

			const float t = IntersectRayTriangle(ray, p);
			if (t < mint) {
				mint = t;
			}

			if (pAllIntersections != nullptr && t != FLT_MAX) {
				pAllIntersections->push_back(t);
			}
		}

		return mint;
	}

} // namespace Model.

} // namespace sge
