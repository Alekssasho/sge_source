#pragma once

#include "sge_utils/sge_utils.h"

#if 0 && defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

namespace sge {

struct Timer
{
	Timer() { initialize(); }

	void initialize()
	{
		__int64 frequency;
		QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
		secondsPerTick = 1.f / (float)(frequency);

		QueryPerformanceCounter((LARGE_INTEGER*)&lastUpdateCnt);
		fps = 0;
		dtSeconds = 0;
	}

	void tick()
	{
		__int64 cnt;

		QueryPerformanceCounter((LARGE_INTEGER*)&cnt);

		const __int64 cntDelta = (cnt - lastUpdateCnt);
		dtSeconds = (float)(cntDelta) * secondsPerTick;
		fps = 1.f / dtSeconds;
		lastUpdateCnt = cnt;
		return;
	}

	float diff_seconds() const { return dtSeconds; }
	float framesPerSecond() const { return fps; }

private :

	float dtSeconds, fps;
	__int64 lastUpdateCnt;
	float secondsPerTick;
};

}

#else

#include <chrono>

namespace sge {

struct Timer {
	typedef std::chrono::high_resolution_clock clock;
	typedef clock::time_point time_point;

	static const time_point application_start_time;

	Timer() { reset(); }

	void reset() {
		lastUpdate = clock::now();
		dtSeconds = 0.f;
	}

	// Returns the current time since the application start in seconds.
	static float now_seconds() {
		return std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - application_start_time).count() * 1e-6f;
	}

	static float currentPointTimeSeconds() {
		return std::chrono::duration_cast<std::chrono::microseconds>(clock::now().time_since_epoch()).count() * 1e-6f;
	}

#if 0
	static double now_seconds_double() {
		return std::chrono::duration_cast<std::chrono::nanoseconds>(clock::now() - application_start_time).count() * 1e-9;
	}
#endif

	bool tick() {
		float minThickLength = maxTicksPerSeconds ? 1.f / maxTicksPerSeconds : 0.f;

		const time_point now = clock::now();
		const auto diff = std::chrono::duration_cast<std::chrono::microseconds>(now - lastUpdate).count();
		dtSeconds = diff * 1e-6f; // microseconds -> seconds

		if (dtSeconds < minThickLength)
			return false;

		lastUpdate = now;
		return true;
	}

	float diff_seconds() const { return dtSeconds; }

	void setFPSCap(float fps) { maxTicksPerSeconds = fps; }

  private:
	time_point lastUpdate;
	float dtSeconds;

	float maxTicksPerSeconds = 0.f;
};

} // namespace sge

#endif
