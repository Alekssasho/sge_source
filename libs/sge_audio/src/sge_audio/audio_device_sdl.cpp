#include "audio_device_sdl.h"

#include <cstring>
#include <sge_utils/sge_utils.h>

// TODO: Find better way
#pragma warning(push)
#pragma warning(disable : 4244)
#pragma warning(disable : 4456)
#pragma warning(disable : 4457)
#pragma warning(disable : 4100)
#pragma warning(disable : 4996)
#pragma warning(disable : 4701)
//#define STB_VORBIS_HEADER_ONLY
#include <stb_vorbis.c>
#pragma warning(pop)

#include "audio_assets.h"

namespace sge {
SGEAudioDeviceSDL::SGEAudioDeviceSDL() {
	SDL_AudioSpec spec;
	memset(&spec, 0, sizeof(spec));
	spec.freq = 48000;
	spec.format = AUDIO_F32;
	spec.channels = 2; // TODO: Detect more than 2 channel setup
	spec.samples = 4096;             /**< Audio buffer size in sample FRAMES (total samples divided by channel count) */
	spec.callback = onAudioCallback; // TODO: For now we queue audio directly.
	spec.userdata = this; // Not used currently as we don't have a callback

	int success = SDL_OpenAudio(&spec, nullptr);
	sgeAssert(success == 0);
}

void SGEAudioDeviceSDL::update(float UNUSED(dt)) {
	//SDL_QueueAudio(1, )
}

void SGEAudioDeviceSDL::setBackgroundMusic(const AudioAsset& backgroundMusic) {
	int vorbisError = 0;
	stb_vorbis* decoder =
	    stb_vorbis_open_memory(reinterpret_cast<const unsigned char*>(backgroundMusic.encodedData.data()), int(backgroundMusic.encodedData.size()), &vorbisError, nullptr);
	if (decoder == nullptr) {
		sgeAssert(decoder != nullptr);
		// TODO: add logging here
	}

	stb_vorbis_info backgroundMusicInfo = stb_vorbis_get_info(m_backgroundVorbisDecoder);
	sgeAssert(backgroundMusicInfo.sample_rate == 48000 && backgroundMusicInfo.channels == 2);
	// TODO: Add logging

	// We are reading this from the other thread so we need to lock it
	SDL_LockAudio();
	if (m_backgroundVorbisDecoder) {
		stb_vorbis_close(m_backgroundVorbisDecoder);
	}
	m_backgroundVorbisDecoder = decoder;
	SDL_UnlockAudio();
}

void SGEAudioDeviceSDL::play() {
	SDL_PauseAudio(0);
}

void SGEAudioDeviceSDL::pause() {
	SDL_PauseAudio(1);
}

void SGEAudioDeviceSDL::onAudioCallback(void* userAudioDevice, Uint8* stream, int len) {
	SGEAudioDeviceSDL* device = reinterpret_cast<SGEAudioDeviceSDL*>(userAudioDevice);
	(void*)device;

	struct AudioFrame {
		float left;
		float right;
	};

	AudioFrame* frames = reinterpret_cast<AudioFrame*>(stream);
	const uint32 numFramesToWrite = len / sizeof(AudioFrame);
	sgeAssert((numFramesToWrite * sizeof(AudioFrame)) == len);

	for (uint32 i = 0; i < numFramesToWrite; ++i) {
		float sineWave = sinf(device->m_SampleCount * 2.0f * float(M_PI) * 110.0f / 48000);
		sineWave *= 0.1f;
		frames[i].left = sineWave;
		frames[i].right = sineWave;
		device->m_SampleCount++;
	}
}
} // namespace sge
