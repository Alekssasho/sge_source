#pragma once

#include "sge_core/sgecore_api.h"
#include "sge_utils/utils/json.h"

namespace sge {

class ParameterBlock;

namespace Model {
	struct Model;
}

class SGE_CORE_API ModelWriter {
  public:
	struct DataChunk {
		DataChunk() = default;

		DataChunk(int id, const void* data, size_t sizeBytes)
		    : id(id)
		    , data(data)
		    , sizeBytes(sizeBytes) {
		}

		int id;
		const void* data;
		size_t sizeBytes;
	};

	ModelWriter() {
	}
	~ModelWriter() {
	}

	bool write(const Model::Model& modelToWrite, IWriteStream* iws);
	bool write(const Model::Model& modelToWrite, const char* const filename);

  private:
	// Returns the chunk id.
	int NewChunkFromPtr(const void* const ptr, const size_t sizeBytes);

	// This function assumes that the vector wont be resized(aka. the data pointer won't change).
	template <typename T>
	int NewChunkFromStdVector(const std::vector<T>& data) {
		return NewChunkFromPtr(data.data(), data.size() * sizeof(T));
	}

	// Genrates (and returns) the json needed, and add the data chunks.
	JsonValue* WriteParamBlock(const ParameterBlock& paramBlock);

	// The actiual writer is implemented in those functions.
	void GenerateAnimations();
	void GenerateNodeHierarchy(); // Adds the "nodeHierarchy" to the root.
	void GenerateNodes();         // Add the "nodes" to the root.
	void GenerateMaterials();     // Add the "materials" to the root.
	void GenerateMeshesData();
	void GenerateCollisionData();

	JsonValueBuffer jvb;
	std::vector<DataChunk> dataChunks; // A list of the data chunks that will end up written to the file.
	JsonValue* root;                   // The file header json element.
	const Model::Model* model;         // The working model
};

} // namespace sge
