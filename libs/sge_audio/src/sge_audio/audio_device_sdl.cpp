#include "audio_device_sdl.h"

#include <cstring>
#include "sge_utils/sge_utils.h"

#include "audio_track.h"

namespace sge {
AudioDeviceSDL::AudioDeviceSDL() {
	SDL_AudioSpec spec;
	memset(&spec, 0, sizeof(spec));
	spec.freq = 48000;
	spec.format = AUDIO_F32;
	spec.channels = 2; // TODO: Detect more than 2 channel setup
	spec.samples = 4096;
	spec.callback = onAudioCallback; // TODO: Consider queue audio as well instead of having callback
	spec.userdata = this;

	int success = SDL_OpenAudio(&spec, nullptr);
	sgeAssert(success == 0);

	// Start playing right away. We will write just silence if we don't have something else to play
	SDL_PauseAudio(0);
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

void AudioDeviceSDL::setMasterVolume(float volume) {
	sgeAssert(volume >= 0.0f && volume <= 1.0f);
	SDL_LockAudio();
	m_masterVolume = volume;
	SDL_UnlockAudio();
}

void AudioDeviceSDL::fillAudioFrames(AudioFrame *frames, uint32_t numFrames) {
	if (!hasAnythingToPlay()) {
		// Fill with silence
		memset(frames, 0, sizeof(AudioFrame) * numFrames);
		return;
	}

	if (m_backgroundMusic) {
		uint32_t numSamplesDecoded = m_backgroundMusic->decoder.decodeSamples(reinterpret_cast<float*>(frames), numFrames);
		// TODO: support different modes here instead of always looping
		if (numSamplesDecoded < numFrames) {
			m_backgroundMusic->decoder.seekToBegining();
			m_backgroundMusic->decoder.decodeSamples(reinterpret_cast<float*>(frames) + numSamplesDecoded * 2, numFrames - numSamplesDecoded);
		}

		// Apply the track volume as well
		for (uint32 i = 0; i < numFrames; ++i) {
			frames[i].left *= m_backgroundMusic->trackVolume;
			frames[i].right *= m_backgroundMusic->trackVolume;
		}
	}

	// Apply master volume
	for (uint32 i = 0; i < numFrames; ++i) {
		frames[i].left *= m_masterVolume;
		frames[i].right *= m_masterVolume;
	}
}

void AudioDeviceSDL::onAudioCallback(void* userAudioDevice, Uint8* stream, int len) {
	AudioDeviceSDL* device = reinterpret_cast<AudioDeviceSDL*>(userAudioDevice);

	AudioFrame* frames = reinterpret_cast<AudioFrame*>(stream);
	const uint32 numFramesToWrite = len / sizeof(AudioFrame);
	sgeAssert((numFramesToWrite * sizeof(AudioFrame)) == len);

	device->fillAudioFrames(frames, numFramesToWrite);
}

bool AudioDeviceSDL::hasAnythingToPlay() {
	return m_backgroundMusic != nullptr;
}
} // namespace sge
