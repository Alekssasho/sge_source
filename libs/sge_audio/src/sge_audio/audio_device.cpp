#include "audio_device.h"
#include "audio_device_sdl.h"
#include "sge_utils/sge_utils.h"

namespace sge {

AudioDevice* AudioDevice::create(const AudioDeviceDesc& UNUSED(deviceDesc)) {
	return new AudioDeviceSDL;
}
}
