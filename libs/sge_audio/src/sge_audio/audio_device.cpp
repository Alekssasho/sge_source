#include "audio_device.h"
#include "audio_device_sdl.h"

namespace sge {

SGEAudioDevice* SGEAudioDevice::create(const AudioDeviceDesc& /*deviceDesc*/) {
	return new SGEAudioDeviceSDL;
}
}
