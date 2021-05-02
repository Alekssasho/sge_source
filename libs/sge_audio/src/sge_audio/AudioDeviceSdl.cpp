#include "AudioDeviceSdl.h"
#include "AudioTrack.h"
#include "sge_utils/math/common.h"
#include "sge_utils/sge_utils.h"
#include "sge_utils/utils/basetypes.h"
#include <cstring>

namespace sge {

/// @brief A wrapper class around the pair of SDL functions
/// SDL_LockAudio and SDL_UnlockAudio.
/// When SDL_LockAudio() is called SDL guarantees that the specified
/// SDL_AudioSpec::callback function WILL NOT get called!
/// Calling SDL_UnlockAudio() will re-enable SDL to call that callback.
struct LockAudioGuard_SDL : public Noncopyable {
	[[nodiscard]] LockAudioGuard_SDL() { SDL_LockAudio(); }
	~LockAudioGuard_SDL() { SDL_UnlockAudio(); }
};

AudioDeviceSDL::AudioDeviceSDL() {
	static_assert(std::is_trivially_copyable<StereoSample>::value);
	static_assert(std::is_trivially_destructible<StereoSample>::value);

	SDL_AudioSpec spec;
	memset(&spec, 0, sizeof(spec));
	spec.freq = 48000;
	spec.format = AUDIO_F32;
	spec.channels = kNumAudioChannels; // TODO: Detect more than 2 channel setup.
	spec.samples = 4096;
	spec.callback = onAudioCallback; // TODO: Consider queue audio as well instead of having callback.
	spec.userdata = this;            // The user data pointer to be passed to @onAudioCallback.

	// According to the SDL2 documentation it is adviced to use the newer SDL_OpenAudioDevice as
	// it provides a control over a specific device (if multiple are available) and the old one is concidered legacy.
	[[maybe_unused]] int success = SDL_OpenAudio(&spec, nullptr);
	sgeAssert(success == 0);

	// Start playing right away. We will write just silence if we don't have something else to play.
	SDL_PauseAudio(0);
}

void AudioDeviceSDL::setBackgroundMusic(const std::shared_ptr<AudioTrack>& backgroundMusic) {
	// @onAudioCallback might be running right now, we need to lock the
	// SDL audio before changeing it.
	const LockAudioGuard_SDL audioLockGuard;
	m_backgroundMusic = backgroundMusic;
}

void AudioDeviceSDL::setMasterVolume(float volume) {
	if (volume < 0.0f || volume > 1.0f) {
		sgeAssert(false && "Volume is expected to be in [0;1] range!");
	}
	volume = clamp01(volume);

	// @onAudioCallback might be running right now, we need to lock the
	// SDL audio before changeing it.
	const LockAudioGuard_SDL audioLockGuard;
	m_masterVolume = volume;
}

void AudioDeviceSDL::onAudioCallback_fillAudioFrames(StereoSample* const frames, const uint32_t numFrames) {
	// If there is nothing to playback, make sure we've output silence.
	if (m_backgroundMusic == nullptr) {
		for (uint32_t i = 0; i < numFrames; ++i) {
			frames[i] = StereoSample();
		}
		return;
	}

	// Decode, sample and advance @m_backgroundMusic.
	const uint32_t numSamplesDecoded = m_backgroundMusic->decoder.decodeSamples(reinterpret_cast<float*>(frames), numFrames);
	// TODO: support different modes here instead of always looping.
	if (numSamplesDecoded < numFrames) {
		m_backgroundMusic->decoder.seekToBegining();

		static_assert(sizeof(StereoSample) == kNumAudioChannels * sizeof(float));
		m_backgroundMusic->decoder.decodeSamples(reinterpret_cast<float*>(frames) + numSamplesDecoded * kNumAudioChannels,
		                                         numFrames - numSamplesDecoded);
	}

	// Apply the track volume and the master volume as well.
	const float totalVolumeMultipler = m_backgroundMusic->trackVolume * m_masterVolume;
	for (uint32 i = 0; i < numFrames; ++i) {
		frames[i].left *= totalVolumeMultipler;
		frames[i].right *= totalVolumeMultipler;
	}
}

void AudioDeviceSDL::onAudioCallback(void* userAudioDevice, Uint8* stream, int streamSizeBytes) {
	// According to the SDL2 documwntation it is save to access memory that is
	// synchronised with SDL_LockAudio, meaning all external reads/writes to those variables
	// need to wait for this callback to finish.
	sgeAssert(streamSizeBytes >= 0);

	AudioDeviceSDL* const device = reinterpret_cast<AudioDeviceSDL*>(userAudioDevice);
	if_checked(device != nullptr && stream != nullptr) {
		StereoSample* const frames = reinterpret_cast<StereoSample*>(stream);
		const uint32 numFramesToWrite = streamSizeBytes / sizeof(StereoSample);

		if ((numFramesToWrite * sizeof(StereoSample)) == streamSizeBytes) {
			device->onAudioCallback_fillAudioFrames(frames, numFramesToWrite);
		} else {
			sgeAssert(false);
		}
	}
}

} // namespace sge
