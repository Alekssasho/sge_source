#pragma once

#include "audio_device.h"
#include <SDL_audio.h>

namespace sge {
struct SGEAudioDeviceSDL : public SGEAudioDevice {
	SGEAudioDeviceSDL();
	~SGEAudioDeviceSDL();

	void update(float dt) final;
private:
};
} // namespace sge
