#pragma once

#include "vec3.h"

namespace sge {

inline float toSphericalYUp(float& angleFromY, float& angleAroundY, const vec3f& d) {
	const float r = d.length();
	angleFromY = acosf(d.y / r);
	angleAroundY = atan2(-d.z, d.x);
	return r;
}

inline vec3f fromSphericalYUp(float angleFromY, float angleAroundY, float radius) {
	vec3f r;
	r.y = radius * cos(angleFromY);
	r.x = radius * cos(angleAroundY) * sin(angleFromY);
	r.z = radius * sin(angleAroundY) * sin(angleFromY);

	return r;
}

} // namespace sge
