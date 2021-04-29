#include "audio_track.h"
#include "audio_decoder.h"

namespace  sge {
AudioTrack::AudioTrack(std::vector<char> data)
	: encodedData(std::move(data))
	, decoder(reinterpret_cast<const unsigned char*>(encodedData.data()), int(encodedData.size()))
	, trackVolume(1.0f)
{
	info = decoder.getTrackInfo();
}
} // namespace sge
