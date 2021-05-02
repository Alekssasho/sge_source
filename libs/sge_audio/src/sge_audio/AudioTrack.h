#pragma once

#include "AudioDecoder.h"
#include <vector>

namespace sge {

struct AudioTrack {
	AudioTrack(std::vector<char> data);

  public:
	/// @brief Holds the audio encoded data (currently vorbis encoding).
	std::vector<char> encodedData;

	VorbisDecoder decoder;
	float trackVolume = 1.f;
	TrackInfo info;
};

} // namespace sge
