#pragma once

#include "Model.h"
#include "sge_core/sgecore_api.h"
#include "sge_renderer/renderer/renderer.h"

namespace sge {

struct JsonValue;

namespace Model {
	class SGE_CORE_API ModelReader {
		struct DataChunkDesc {
			int chunkId = 0;
			size_t byteOffset = 0;
			size_t sizeBytes = 0;
		};

	  public:
		static PrimitiveTopology::Enum PrimitiveTolologyFromString(const char* str);
		static UniformType::Enum UniformTypeFromString(const char* str);

		ModelReader() {}
		~ModelReader() {}

		bool Load(const LoadSettings loadSets, SGEDevice* sgedev, IReadStream* const irs, Model& model);

	  private:
		IReadStream* irs;
		std::vector<DataChunkDesc> dataChunksDesc;

		const DataChunkDesc& FindDataChunkDesc(const int chunkId) const;

		// CAUTION: These functions assume that irs points at the BEGINING of data chunks.
		template <typename T>
		void LoadDataChunk(std::vector<T>& resultBuffer, const int chunkId);
		void LoadDataChunkRaw(void* const ptr, const size_t ptrExpectedSize, const int chunkId);
		bool LoadParamBlock(const JsonValue* jParamBlock, ParameterBlock& paramBlock);
	};

} // namespace Model

} // namespace sge
