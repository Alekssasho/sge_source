#pragma once

#include "vec4.h"

namespace sge {

inline uint32 colorIntFromRGBA255(ubyte r, ubyte g, ubyte b, ubyte a = 255) {
	uint32 c = 0;
	c |= (int)(r) << 0;
	c |= (int)(g) << 8;
	c |= (int)(b) << 16;
	c |= (int)(a) << 24;
	return c;
};

inline uint32 colorToIntRgba(float r, float g, float b, float a = 1.f) {
	r = clamp01(r);
	g = clamp01(g);
	b = clamp01(b);
	a = clamp01(a);

	uint32 c = 0;
	c |= (int)(r * 255) << 0;
	c |= (int)(g * 255) << 8;
	c |= (int)(b * 255) << 16;
	c |= (int)(a * 255) << 24;
	return c;
};

inline uint32 colorToIntRgba(const vec4f& rgba) {
	return colorToIntRgba(rgba.x, rgba.y, rgba.z, rgba.w);
};

inline uint32 colorToIntRgba(const vec3f& rgb, float a = 1.f) {
	return colorToIntRgba(rgb.x, rgb.y, rgb.z, a);
};

inline vec4f colorFromIntRgba(uint32 rgba) {
	float r = float(rgba & 0xFF) / 255.f;
	float g = float((rgba >> 8) & 0xFF) / 255.f;
	float b = float((rgba >> 16) & 0xFF) / 255.f;
	float a = float((rgba >> 24) & 0xFF) / 255.f;

	return vec4f(r, g, b, a);
}

inline vec4f colorFromIntRgba(ubyte r255, ubyte g255, ubyte b255, ubyte a255 = 255) {
	float r = float(r255) / 255.f;
	float g = float(g255) / 255.f;
	float b = float(b255) / 255.f;
	float a = float(a255) / 255.f;

	return vec4f(r, g, b, a);
}

inline vec4f colorBlack(float alpha) {
	return vec4f(0.f, 0.f, 0.f, alpha);
}

inline vec4f colorWhite(float alpha) {
	return vec4f(1.f, 1.f, 1.f, alpha);
}


} // namespace sge
