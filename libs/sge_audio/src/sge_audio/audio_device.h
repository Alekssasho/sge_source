#pragma once
#include <memory>
#include <sge_audio/audio_track.h>

namespace sge {
struct AudioDeviceDesc {

};

struct AudioDevice {
	static AudioDevice* create(const AudioDeviceDesc& deviceDesc);

	virtual void update(float dt) = 0;
	virtual void setBackgroundMusic(AudioTrack* backgroundMusic) = 0;
	virtual void play() = 0;
	virtual void pause() = 0;
};
} // namespace sge
