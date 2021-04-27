#pragma once
#include <memory>
#include <sge_audio/audio_assets.h>

namespace sge {
struct Asset;

struct AudioDeviceDesc {

};

struct SGEAudioDevice {
	static SGEAudioDevice* create(const AudioDeviceDesc& deviceDesc);

	virtual void update(float dt) = 0;
	virtual void setBackgroundMusic(const AudioAsset& backgroundMusic) = 0;
	virtual void play() = 0;
	virtual void pause() = 0;
};
} // namespace sge
