#include "input.h"

namespace sge{

InputState::InputState()
{
	m_cursorClient = vec2f(0.f);
	m_cursorDomain = vec2f(0.f);
	m_cursorMotion = vec2f(0.f);
	m_wheelCount = 0;
	for(auto& v : m_keyStates) v = 0;

	for(int t = 0; t < SGE_ARRSZ(winapiGamepads); ++t) {
		//winapiGamepads[t].isUsingStatePolling = false;
	}
}

// Moves the crrent input state into previous

void InputState::Advance()
{
	m_hadkeyboardOrMouseInputThisPoll = false;
	m_wasActiveWhilePolling = false;
	m_inputText.clear();
	m_wheelCount = 0;
	// Advance the polled input one step ahead, while keeping the current.
	for(auto& state : m_keyStates) {
		state = (state << 1) | (state & 1);
	}

	for(int t = 0; t < SGE_ARRSZ(xinputDevicesState); ++t) {
		xinputDevicesState[t].Advance(false);
	}

	for(int t = 0; t < SGE_ARRSZ(winapiGamepads); ++t) {
		winapiGamepads[t].Advance(false);
	}

	m_cursorMotion = vec2f(0.f);
}
const GamepadState* InputState::getHookedGemepad(const int playerIndex) const {
	int numFoundHooked = 0;
	for(int t = 0; t < SGE_ARRSZ(xinputDevicesState); ++t) {
		if(xinputDevicesState[t].hooked)
		{
			if(numFoundHooked == playerIndex) {
				return &xinputDevicesState[t];
			}
			numFoundHooked++;
		}
	}

	for(int t = 0; t < SGE_ARRSZ(winapiGamepads); ++t) {
		if(winapiGamepads[t].hooked)
		{
			if(numFoundHooked == playerIndex) {
				return &winapiGamepads[t];
			}
			numFoundHooked++;
		}
	}

	return nullptr;
}

void GamepadState::Advance(bool sdlStyleDontZeroTheThumbsticks)
{
	hadInputThisPoll = false;
	hooked = false;

	btnA = (btnA << 1) | (btnA & 1);
	btnB = (btnB << 1) | (btnB & 1);
	btnX = (btnX << 1) | (btnX & 1);
	btnY = (btnY << 1) | (btnY & 1);
	btnShoulderL = (btnShoulderL << 1) | (btnShoulderL & 1);
	btnShoulderR = (btnShoulderR << 1) | (btnShoulderR & 1);

	btnUp = (btnUp << 1) | (btnUp & 1);
	btnDown = (btnDown << 1) | (btnDown & 1);
	btnLeft = (btnLeft << 1) | (btnLeft & 1);
	btnRight = (btnRight << 1) | (btnRight & 1);

	btnThumbL = (btnThumbL << 1) | (btnThumbL & 1);
	btnThumbR = (btnThumbR << 1) | (btnThumbR & 1);

	btnBack = (btnBack << 1) | (btnBack & 1);
	btnStart = (btnStart << 1) | (btnStart & 1);

	if (sdlStyleDontZeroTheThumbsticks) {
		axisL = vec2f(0.f);
		axisR = vec2f(0.f);
		triggerL = 0.f;
		triggerR = 0.f;
	}
}

vec2f GamepadState::getInputDir(bool const includeDPad) const
{
	vec2f r(0.f);

	if(!hooked) return r;

	if(includeDPad)
	{
		if(btnLeft & 0x1) r.x = -1.f;
		if(btnRight & 0x1) r.x = 1.f;

		if(btnUp & 0x1) r.y = 1.f;
		if(btnDown & 0x1) r.y = -1.f;
	}

	if(r.x == 0.f && r.y == 0.f)
	{
		r = axisL;
	}

	return r;
}

}
