#include "audio_decoder.h"

#include "sge_utils/sge_utils.h"

SGE_NO_WARN_BEGIN
#pragma warning(disable : 4701)
//#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
SGE_NO_WARN_END

namespace sge {
VorbisDecoder::VorbisDecoder(const unsigned char *data, int dataSizeInBytes) {
	int vorbisError = 0;
	m_decoder = stb_vorbis_open_memory(data, dataSizeInBytes, &vorbisError, nullptr);
	if (m_decoder == nullptr) {
		sgeAssert(m_decoder != nullptr);
		// TODO: add logging here
	}
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
	  result.channels = trackInfo.channels;
	  result.sampleRate = trackInfo.sample_rate;
	  result.numSamples = stb_vorbis_stream_length_in_samples(m_decoder);
	}
	return result;
}

uint32_t VorbisDecoder::decodeSamples(float *data, int numSamplesToDecode) {
	const int numChannels = 2;
	uint32_t samplesDecoded = stb_vorbis_get_samples_float_interleaved(m_decoder, numChannels, data, numChannels * numSamplesToDecode);
	return samplesDecoded;
}

void VorbisDecoder::seekToBegining() {
	stb_vorbis_seek_start(m_decoder);
}
} // namespace sge
