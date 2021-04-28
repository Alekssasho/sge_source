#pragma once

#include <cstdint>
struct stb_vorbis;

namespace sge {

struct TrackInfo {
	uint32_t sampleRate;
	uint32_t numSamples;
	uint32_t channels;
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
