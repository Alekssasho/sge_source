#include "AudioDevice.h"
#include "AudioDeviceSdl.h"
#include "sge_utils/sge_utils.h"

namespace sge {

AudioDevice* AudioDevice::create(const AudioDeviceDesc& UNUSED(deviceDesc)) {
	return new AudioDeviceSDL;
}
} // namespace sge
