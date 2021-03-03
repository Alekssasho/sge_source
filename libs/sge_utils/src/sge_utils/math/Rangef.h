#pragma once

#include "sge_utils/sge_utils.h"

namespace sge {

struct Rangef {
	Rangef() = default;

	Rangef(float minAndMax) {
		this->locked = true;
		this->min = minAndMax;
		this->max = minAndMax;
	}

	Rangef(float min, float max) {
		sgeAssert(min <= max);
		this->locked = false;
		this->min = min;
		this->max = min > max ? min : max;
	}

	float sample(const float v) const {
		if (locked) {
			return min;
		}

		return min + v * (max - min);
	}

	bool operator==(const Rangef& ref) const {
		return min == ref.min && max == ref.max && locked == ref.locked;
	}

	bool operator!=(const Rangef& ref) const {
		return !(*this == ref);
	}

	float min = 0.f;
	float max = 0.f;

	bool locked = false; // if locked then only the min value should be used.
};

} // namespace sge
