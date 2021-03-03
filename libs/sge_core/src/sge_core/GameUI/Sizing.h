#pragma once

#include "sge_utils/math/Box.h"
#include "sge_utils/math/vec4.h"
#include "sge_utils/utils/optional.h"

namespace sge::gamegui {

enum UnitMeasure : int {
	unitMeasure_pixels,
	unitMeasure_fraction,  // same as percentage but in range [0;1]
	unitMeasure_wFraction, // fraction but from the width of the parent widget
	unitMeasure_hFraction, // fraction but from the height of the parent widget
};

struct Unit {
	static Unit fromFrac(float c) {
		Unit r;
		r.mode = unitMeasure_fraction;
		r.value = c;
		return r;
	}

	static Unit fromWFrac(float c) {
		Unit r;
		r.mode = unitMeasure_wFraction;
		r.value = c;
		return r;
	}

	static Unit fromHFrac(float c) {
		Unit r;
		r.mode = unitMeasure_hFraction;
		r.value = c;
		return r;
	}

	static Unit fromPixels(float c) {
		Unit r;
		r.mode = unitMeasure_pixels;
		r.value = c;
		return r;
	}

	float computeSizePixels(const bool isXDim, const vec2f& dim) const {
		switch (mode) {
			case unitMeasure_pixels: {
				return value;
			}
			case unitMeasure_fraction: {
				float result = value * (isXDim ? dim.x : dim.y);
				return result;
			}
			case unitMeasure_hFraction: {
				float result = value * dim.y;
				return result;
			}
			case unitMeasure_wFraction: {
				float result = value * dim.x;
				return result;
			}
			default:
				sgeAssert(false && "Unimplemented unit mode!");
				return 0.f;
		}
	}

	UnitMeasure mode = unitMeasure_pixels;
	float value = 0.f;
};

struct Size {
	Optional<Unit> minSizeX;
	Optional<Unit> minSizeY;
	Unit sizeX;
	Unit sizeY;

	Size() = default;

	Size(const Unit& x, const Unit& y)
	    : sizeX(x)
	    , sizeY(y) {}

	static Size fromPixels(float x, float y) {
		Size r;
		r.sizeX = Unit::fromPixels(x);
		r.sizeY = Unit::fromPixels(y);
		return r;
	}

	static Size fromFrac(const vec2f& sizeCoeff) {
		Size r;
		r.sizeX = Unit::fromFrac(sizeCoeff.x);
		r.sizeY = Unit::fromFrac(sizeCoeff.y);
		return r;
	}

	static Size fromHFrac(const vec2f& sizeCoeff) {
		Size r;
		r.sizeX = Unit::fromHFrac(sizeCoeff.x);
		r.sizeY = Unit::fromHFrac(sizeCoeff.y);
		return r;
	}

	static Size fromHFrac(float x, float y) {
		Size r;
		r.sizeX = Unit::fromHFrac(x);
		r.sizeY = Unit::fromHFrac(y);
		return r;
	}

	vec2f computeSizePixels(const vec2f& dimensionsSize) const {
		float xss = sizeX.computeSizePixels(true, dimensionsSize);
		float yss = sizeY.computeSizePixels(false, dimensionsSize);

		if (minSizeX.hasValue()) {
			float minx = minSizeX->computeSizePixels(true, dimensionsSize);
			xss = maxOf(minx, xss);
		}

		if (minSizeY.hasValue()) {
			float miny = minSizeY->computeSizePixels(false, dimensionsSize);
			yss = maxOf(miny, yss);
		}

		return vec2f(xss, yss);
	}
};

struct Pos {
	vec2f anchorCoeff = vec2f(0.f);
	Unit posX;
	Unit posY;

	Pos() = default;

	Pos(const Unit& x, const Unit& y, const vec2f& anchorCoeff = vec2f(0.f))
	    : posX(x)
	    , posY(y)
	    , anchorCoeff(anchorCoeff) {}

	static Pos fromFrac(float x, float y, vec2f anchor = vec2f(0.f)) {
		Pos result;
		result.anchorCoeff = anchor;
		result.posX = Unit::fromFrac(x);
		result.posY = Unit::fromFrac(y);
		return result;
	}

	static Pos fromHFrac(float x, float y, vec2f anchor = vec2f(0.f)) {
		Pos result;
		result.anchorCoeff = anchor;
		result.posX = Unit::fromHFrac(x);
		result.posY = Unit::fromHFrac(y);
		return result;
	}

	/// Computes the bounding box of the widget represnted by this poistioning.
	/// @param [in] parentBBox is the size in pixels of the parent box that contains the current position.
	/// @param [in] size is the size associated with this position.
	/// @result is the box containg the recatangle represented by this position and @size in pixels.
	AABox2f getBBoxPixels(const AABox2f& parentBBox, vec2f parentContentOriginPixels, const Size& size) const {
		const vec2f parentBBoxSizePixels = parentBBox.size();
		const vec2f anchorPosSS(posX.computeSizePixels(true, parentBBoxSizePixels), posY.computeSizePixels(false, parentBBoxSizePixels));
		const vec2f sizeSS = size.computeSizePixels(parentBBox.size());
		const vec2f posTopLeftSS = anchorPosSS - sizeSS * anchorCoeff + parentBBox.min + parentContentOriginPixels;

		AABox2f bboxPixels;
		bboxPixels.expand(posTopLeftSS);
		bboxPixels.expand(posTopLeftSS + sizeSS);

		return bboxPixels;
	}

	vec2f toPixels(const vec2f& parentBBoxSizePixels) const {
		const vec2f posSS(posX.computeSizePixels(true, parentBBoxSizePixels), posY.computeSizePixels(false, parentBBoxSizePixels));
		return posSS;
	}

	///
	Pos getAsFraction(const vec2f& dimensionsSize) const {
		float xPixels = posX.computeSizePixels(true, dimensionsSize);
		float yPixels = posY.computeSizePixels(false, dimensionsSize);
		return Pos::fromFrac(dimensionsSize.x ? xPixels / dimensionsSize.x : 0.f, dimensionsSize.y ? yPixels / dimensionsSize.y : 0);
	}
};

namespace literals {
	inline Unit operator"" _px(const long double v) { return Unit::fromPixels(float(v)); }
	inline Unit operator"" _px(const unsigned long long v) { return Unit::fromPixels(float(v)); }

	inline Unit operator"" _f(const long double v) { return Unit::fromFrac(float(v)); }
	inline Unit operator"" _f(const unsigned long long v) { return Unit::fromFrac(float(v)); }

	inline Unit operator"" _hf(const long double v) { return Unit::fromHFrac(float(v)); }
	inline Unit operator"" _hf(const unsigned long long v) { return Unit::fromHFrac(float(v)); }

	inline Unit operator"" _wf(const long double v) { return Unit::fromWFrac(float(v)); }
	inline Unit operator"" _wf(const unsigned long long v) { return Unit::fromWFrac(float(v)); }
} // namespace literals
} // namespace sge::gamegui
