#pragma once

#include "AudioDevice.h"
#include <SDL_audio.h>

struct stb_vorbis;

namespace sge {
struct AudioDeviceSDL : public AudioDevice {
	AudioDeviceSDL();
	~AudioDeviceSDL() = default;

	void setBackgroundMusic(const std::shared_ptr<AudioTrack>& backgroundMusic) final;
	void setMasterVolume(float volume) final;

  private:
	struct StereoSample {
		StereoSample() = default;
		float left = 0.f;
		float right = 0.f;
	};

	void onAudioCallback_fillAudioFrames(StereoSample* const frames, const uint32_t numFrames);
	static void onAudioCallback(void* userdata, Uint8* stream, int len);

	/// Background music track to be player.
	std::shared_ptr<AudioTrack> m_backgroundMusic = nullptr;
	float m_masterVolume = 1.0f;
};
} // namespace sge
