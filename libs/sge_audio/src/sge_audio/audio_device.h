#pragma once

namespace sge {

struct AudioDeviceDesc {

};

struct SGEAudioDevice {
	static SGEAudioDevice* create(const AudioDeviceDesc& deviceDesc);

	virtual void update(float dt) = 0;
};
} // namespace sge
