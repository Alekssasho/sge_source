#pragma once

namespace sge {

struct AudioDeviceDesc {

};

struct SGEAudioDevice {
	static SGEAudioDevice* create(const AudioDeviceDesc& deviceDesc);
};
} // namespace sge
