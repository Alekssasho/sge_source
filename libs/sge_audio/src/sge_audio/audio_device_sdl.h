#pragma once

#include "audio_device.h"
#include <SDL_audio.h>

struct stb_vorbis;

namespace sge {
struct AudioDeviceSDL : public AudioDevice {
	AudioDeviceSDL();
	~AudioDeviceSDL();

	void update(float dt) final;
	void setBackgroundMusic(AudioTrack* backgroundMusic) final;

	void play() final;
	void pause() final;
private:
        // TODO: This probably is not enough if we need more than stereo
	struct AudioFrame {
		float left;
		float right;
	};
	void fillAudioFrames(AudioFrame* frames, uint32_t numFrames);
	static void onAudioCallback(void* userdata, Uint8* stream, int len);
	uint64_t m_SampleCount = 0;

	// Background music
	AudioTrack* m_backgroundMusic = nullptr;
};
} // namespace sge
