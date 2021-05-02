#pragma once
#include <memory>
#include "sge_audio/AudioTrack.h"

namespace sge {
struct AudioDeviceDesc {};

/// @brief AudioDevice handles playing audio tracks and sound effects.
struct AudioDevice {
	enum : int {
		/// The number of audio channel supported. (Basically two channels means two speakers - left and right).
		kNumAudioChannels = 2
	};

	AudioDevice() = default;
	virtual ~AudioDevice() = default;

	static AudioDevice* create(const AudioDeviceDesc& deviceDesc);

	virtual void setBackgroundMusic(const std::shared_ptr<AudioTrack>& backgroundMusic) = 0;

	/// @brief Specifies a multipler to be applied when playing back any kind of sound.
	/// @param volume a value in [0;1] range specifying the volume multipler.
	virtual void setMasterVolume(float volume) = 0;
};
} // namespace sge
