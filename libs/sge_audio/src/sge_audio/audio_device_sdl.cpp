#include "audio_device_sdl.h"

namespace sge {
SGEAudioDeviceSDL::SGEAudioDeviceSDL() {
	SDL_AudioSpec spec;
	SDL_OpenAudio(&spec, nullptr);
}

void SGEAudioDeviceSDL::update(float /*dt*/) {
}
} // namespace sge
