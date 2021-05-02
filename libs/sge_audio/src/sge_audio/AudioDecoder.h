#pragma once

#include <cstdint>
struct stb_vorbis;

namespace sge {

struct TrackInfo {
	/// samplesPerSecond specifies the number of samples per second. Usually called "sample rate".
	uint32_t samplesPerSecond = 0;
	/// numSamples specifies the total amount of samples in the whole track.
	uint32_t numSamples = 0;
	/// numChannles specifies the the number of channel in each sample.
	uint32_t numChannels = 0;
	
	/// @brief Computes the length of the track (song or sound effect for example) in seconds.
	float getLengthInSeconds() const {
		if (samplesPerSecond > 0.f) {
			return float(numSamples) / float(samplesPerSecond);
		}

		return 0.f;
	}
};

struct VorbisDecoder {
	VorbisDecoder(const unsigned char* const data, int dataSizeBytes);
	~VorbisDecoder();
	TrackInfo getTrackInfo();

	uint32_t decodeSamples(float* const outDecodedSamples, const int numSamplesToDecode);
	void seekToBegining();
  private:
	stb_vorbis* m_decoder = nullptr;
};
} // namespace sge
