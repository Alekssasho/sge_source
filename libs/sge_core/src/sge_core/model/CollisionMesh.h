#pragma once

#include "sge_core/sgecore_api.h"
#include "sge_utils/math/vec3.h"
#include <vector>

namespace sge {
namespace Model {
	/// CollisionMesh stores the data extracted from the 3D file format
	/// used for physics and navmesh geometry. Both convex and concave meshes are represented with this one
	/// it is up to the artist to have them actually be convex.
	struct SGE_CORE_API CollisionMesh {
		CollisionMesh() = default;
		CollisionMesh(std::vector<vec3f> vertices, std::vector<int> indices)
		    : vertices(std::move(vertices))
		    , indices(std::move(indices)) {
		}

		void freeMemory() {
			vertices = std::vector<vec3f>();
			indices = std::vector<int>();
		}

		// static CollisionMesh getCombiendMesh(const std::vector<CollisionMesh>& collisionMeshes);

		// Stores a triangle list.
		std::vector<vec3f> vertices;
		std::vector<int> indices;
	};

} // namespace Model
} // namespace sge
