#include "audio_device_sdl.h"

#include <cstring>
#include <sge_utils/sge_utils.h>

#include "audio_track.h"

namespace sge {
AudioDeviceSDL::AudioDeviceSDL() {
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

void AudioDeviceSDL::update(float UNUSED(dt)) {
	//SDL_QueueAudio(1, )
}

void AudioDeviceSDL::setBackgroundMusic(AudioTrack* backgroundMusic) {
	// We are reading this from the other thread so we need to lock it
	SDL_LockAudio();
	m_backgroundMusic = backgroundMusic;
	SDL_UnlockAudio();
}

void AudioDeviceSDL::play() {
	SDL_PauseAudio(0);
}

void AudioDeviceSDL::pause() {
	SDL_PauseAudio(1);
}

void AudioDeviceSDL::fillAudioFrames(AudioFrame *frames, uint32_t numFrames) {
	if (m_backgroundMusic) {
		uint32_t numSamplesDecoded = m_backgroundMusic->decoder.decodeSamples(reinterpret_cast<float*>(frames), numFrames);
		  // TODO: support different modes here instead of always looping
		if (numSamplesDecoded < numFrames) {
			m_backgroundMusic->decoder.seekToBegining();
			m_backgroundMusic->decoder.decodeSamples(reinterpret_cast<float*>(frames) + numSamplesDecoded * 2, numFrames - numSamplesDecoded);
		}
	}
}

void AudioDeviceSDL::onAudioCallback(void* userAudioDevice, Uint8* stream, int len) {
	AudioDeviceSDL* device = reinterpret_cast<AudioDeviceSDL*>(userAudioDevice);

	AudioFrame* frames = reinterpret_cast<AudioFrame*>(stream);
	const uint32 numFramesToWrite = len / sizeof(AudioFrame);
	sgeAssert((numFramesToWrite * sizeof(AudioFrame)) == len);

	device->fillAudioFrames(frames, numFramesToWrite);

	//for (uint32 i = 0; i < numFramesToWrite; ++i) {
	//	float sineWave = sinf(device->m_SampleCount * 2.0f * float(M_PI) * 110.0f / 48000);
	//	sineWave *= 0.1f;
	//	frames[i].left = sineWave;
	//	frames[i].right = sineWave;
	//	device->m_SampleCount++;
	//}
}
} // namespace sge
