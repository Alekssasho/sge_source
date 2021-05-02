#include "AudioDecoder.h"
#include "AudioDevice.h"
#include "sge_utils/sge_utils.h"

SGE_NO_WARN_BEGIN
#pragma warning(disable : 4701) // "C4701: potentially uninitialized local variable '<name>' used" this is generated from stb_vorbis.h.
#include <stb_vorbis.c>
SGE_NO_WARN_END

namespace sge {
VorbisDecoder::VorbisDecoder(const unsigned char* const data, int dataSizeInBytes) {
	int vorbisError = 0;
	m_decoder = stb_vorbis_open_memory(data, dataSizeInBytes, &vorbisError, nullptr);
	sgeAssert(m_decoder != nullptr);
}

VorbisDecoder::~VorbisDecoder() {
	if (m_decoder != nullptr) {
		stb_vorbis_close(m_decoder);
		m_decoder = nullptr;
	}
}

TrackInfo VorbisDecoder::getTrackInfo() {
	TrackInfo result = {0};
	if (m_decoder) {
		stb_vorbis_info trackInfo = stb_vorbis_get_info(m_decoder);
		result.numChannels = trackInfo.channels;
		result.samplesPerSecond = trackInfo.sample_rate;
		result.numSamples = stb_vorbis_stream_length_in_samples(m_decoder);
	}
	return result;
}

uint32_t VorbisDecoder::decodeSamples(float* const outDecodedSamples, const int numSamplesToDecode) {
	if (m_decoder) {
		const uint32_t samplesDecoded = stb_vorbis_get_samples_float_interleaved(
		    m_decoder, AudioDevice::kNumAudioChannels, outDecodedSamples, AudioDevice::kNumAudioChannels * numSamplesToDecode);
		return samplesDecoded;
	}

	return 0;
}

void VorbisDecoder::seekToBegining() {
	if (m_decoder) {
		stb_vorbis_seek_start(m_decoder);
	}
}

} // namespace sge
