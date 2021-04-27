#pragma once

#include "audio_device.h"
#include <SDL_audio.h>

struct stb_vorbis;

namespace sge {
struct AudioAsset;

struct SGEAudioDeviceSDL : public SGEAudioDevice {
	SGEAudioDeviceSDL();
	~SGEAudioDeviceSDL();

	void update(float dt) final;
	void setBackgroundMusic(const AudioAsset& backgroundMusic) final;

	void play() final;
	void pause() final;
private:
	static void onAudioCallback(void* userdata, Uint8* stream, int len);
	uint64_t m_SampleCount = 0;

	// Background music
	stb_vorbis* m_backgroundVorbisDecoder = nullptr;
};
} // namespace sge
