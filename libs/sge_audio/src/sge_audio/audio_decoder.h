#pragma once

#include <cstdint>
struct stb_vorbis;

namespace sge {

struct TrackInfo {
	uint32_t sampleRate = 0;
	uint32_t numSamples = 0;
	uint32_t channels = 0;
};

struct VorbisDecoder {
	VorbisDecoder(const unsigned char* data, int len);
	~VorbisDecoder();
	TrackInfo getTrackInfo();

	uint32_t decodeSamples(float* data, int numSamplesToDecode);
	void seekToBegining();
  private:
	stb_vorbis* m_decoder;
};
} // namespace sge
