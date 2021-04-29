#pragma once
#include <vector>

#include "audio_decoder.h"

struct stb_vorbis;

namespace sge {
struct AudioTrack {
	AudioTrack(std::vector<char> data);

	std::vector<char> encodedData;
	VorbisDecoder decoder;

	float trackVolume;

	// Info
	TrackInfo info;
};
} // namespace sge
