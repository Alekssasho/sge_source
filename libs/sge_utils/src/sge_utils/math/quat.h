#pragma once

#include "common.h"
#include "math_base.h"
#include "vec3.h"

namespace sge {

/////////////////////////////////////////////////////////////////
// Quaternion
// data layout is (x,y,z,w) where :
//(x,y,z) is the imaginary part
// w is the real part
/////////////////////////////////////////////////////////////////
template <typename TDATA_TYPE>
struct quat {
	typedef TDATA_TYPE DATA_TYPE;
	typedef quat SELF_TYPE;

	static const unsigned NUM_ELEMS = 4;

	union {
		struct {
			DATA_TYPE x, y, z, w;
		};
		DATA_TYPE data[NUM_ELEMS];
	};

	//-----------------------------------------------------------
	// constructors
	//-----------------------------------------------------------
	quat() {}

	quat(const DATA_TYPE& sx, const DATA_TYPE& sy, const DATA_TYPE& sz, const DATA_TYPE& sw) {
		data[0] = sx;
		data[1] = sy;
		data[2] = sz;
		data[3] = sw;
	}

	quat(const vec3<DATA_TYPE>& v, DATA_TYPE sw) {
		data[0] = v[0];
		data[1] = v[1];
		data[2] = v[2];
		w = sw;
	}

	//-----------------------------------------------------------
	static quat getAxisAngle(const vec3<DATA_TYPE>& axis, const DATA_TYPE& angle) {
		DATA_TYPE s, c;
		SinCos(angle * (DATA_TYPE)(0.5), s, c);

		quat result;

		result[0] = s * axis[0];
		result[1] = s * axis[1];
		result[2] = s * axis[2];
		result[3] = c;

		return result;
	}

	//-----------------------------------------------------------
	static quat getIdentity() { return quat(0, 0, 0, (DATA_TYPE)(1.0)); }


	//-----------------------------------------------------------
	// conjugate
	//-----------------------------------------------------------
	quat conjugate() const { return quat(-data[0], -data[1], -data[2], data[3]); }

	friend quat conjugate(const quat& q) { return q.conjugate(); }

	//-----------------------------------------------------------
	//
	//-----------------------------------------------------------
	friend DATA_TYPE hsum(const quat& q) { return q.data[0] + q.data[1] + q.data[2] + q.data[3]; }

	friend quat cmul(const quat& a, const quat& b) {
		quat q;

		q.x = a.x * b.x;
		q.y = a.y * b.y;
		q.z = a.z * b.z;
		q.w = a.w * b.w;

		return q;
	}

	vec3<DATA_TYPE> xyz() const { return vec3<DATA_TYPE>(x, y, z); }

	//-----------------------------------------------------------
	// length
	//-----------------------------------------------------------
	DATA_TYPE lengthSqr() const {
		DATA_TYPE result = 0;

		for (unsigned int t = 0; t < 4; ++t) {
			result += data[t] * data[t];
		}

		return result;
	}

	DATA_TYPE length() const {
		// [TODO] fix that
		return sqrtf(lengthSqr());
	}

	friend quat length(const quat& q) { return q.Length(); }

	//-----------------------------------------------------------
	// normalized
	//-----------------------------------------------------------
	quat normalized() const {
		const DATA_TYPE invLength = DATA_TYPE(1.0) / length();

		quat result;
		for (unsigned t = 0; t < 4; ++t) {
			result[t] = data[t] * invLength;
		}

		return result;
	}

	friend quat normalized(const quat& q) { return q.normalized(); }

	//-----------------------------------------------------------
	// inverse
	//-----------------------------------------------------------
	quat inverse() const {
		const DATA_TYPE invLensqr = (DATA_TYPE)1.0 / lengthSqr();
		return conjugate() * invLensqr;
	}

	friend quat inverse(const quat& q) { return q.inverse(); }

	//---------------------------------------------------------
	//
	//---------------------------------------------------------
	bool operator==(const quat& ref) const {
		return data[0] == ref.data[0] && data[1] == ref.data[1] && data[2] == ref.data[2] && data[3] == ref.data[3];
	}

	bool operator!=(const quat& ref) const { return !((*this) == ref); }

	//-----------------------------------------------------------
	// operator[]
	//-----------------------------------------------------------
	DATA_TYPE& operator[](const unsigned int t) { return data[t]; }
	const DATA_TYPE& operator[](const unsigned int t) const { return data[t]; }

	//-----------------------------------------------------------
	// quat + quat
	//-----------------------------------------------------------
	quat& operator+=(const quat& v) {
		for (unsigned int t = 0; t < 4; ++t) {
			data[t] += v[t];
		}
		return *this;
	}

	quat operator+(const quat& v) const {
		quat r;
		for (unsigned int t = 0; t < 4; ++t) {
			r[t] = data[t] + v[t];
		}
		return r;
	}

	//---------------------------------------------------------
	// quat - quat
	//---------------------------------------------------------
	quat& operator-=(const quat& v) {
		for (unsigned int t = 0; t < 4; ++t) {
			data[t] -= v[t];
		}
		return *this;
	}


	quat operator-(const quat& v) const {
		SELF_TYPE r;
		for (unsigned int t = 0; t < 4; ++t) {
			r[t] = data[t] - v[t];
		}
		return r;
	}


	//-----------------------------------------------------------
	// Quat * Scalar
	//-----------------------------------------------------------
	quat operator*=(const DATA_TYPE& s) {
		for (unsigned int t = 0; t < 4; ++t) {
			data[t] *= s;
		}
		return *this;
	}

	quat operator*(const DATA_TYPE& s) const {
		quat r;
		for (unsigned int t = 0; t < 4; ++t) {
			r[t] = data[t] * s;
		}
		return r;
	}

	friend quat operator*(const DATA_TYPE& s, const quat& q) { return q * s; }

	quat operator-() const { return quat(-data[0], -data[1], -data[2], -data[3]); }

	//-----------------------------------------------------------
	// Quat / Scalar
	//-----------------------------------------------------------
	quat operator/=(const DATA_TYPE& s) {
		for (unsigned int t = 0; t < 4; ++t) {
			data[t] /= s;
		}
		return *this;
	}

	quat operator/(const DATA_TYPE& s) const {
		quat r;
		for (unsigned int t = 0; t < 4; ++t) {
			r[t] = data[t] / s;
		}
		return r;
	}

	friend quat operator/(const DATA_TYPE& s, const quat& q) { return q / s; }

	//-----------------------------------------------------------
	// multiplication
	//-----------------------------------------------------------
	friend quat operator*(const quat& p, const quat& q) {
		quat r;

		quat v(p[3], -p[2], p[1], p[0]);
		r[0] = hsum(cmul(v, q));

		v = quat(p[2], p[3], -p[0], p[1]);
		r[1] = hsum(cmul(v, q));

		v = quat(-p[1], p[0], p[3], p[2]);
		r[2] = hsum(cmul(v, q));

		v = quat(-p[0], -p[1], -p[2], p[3]);
		r[3] = hsum(cmul(v, q));

		return r;
	}

	//-----------------------------------------------------------
	//
	//-----------------------------------------------------------
	friend vec3<DATA_TYPE> quat_mul_pos(const quat& q, const vec3<DATA_TYPE>& p) { return (q * quat(p, 0) * q.inverse()).xyz(); }

	friend vec3<DATA_TYPE> normalizedQuat_mul_pos(const quat& q, const vec3<DATA_TYPE>& p) {
		return (q * quat(p, 0) * q.conjugate()).xyz();
	}

	//-----------------------------------------------------------
	//
	//-----------------------------------------------------------
	DATA_TYPE dot(const quat& q) const {
		DATA_TYPE r = 0;
		for (unsigned int t = 0; t < 4; ++t) {
			r += data[t] * q.data[t];
		}
		return r;
	}

	friend DATA_TYPE dot(const quat& q0, const quat& q1) { return q0.dot(q1); }

	//-----------------------------------------------------------
	//
	//-----------------------------------------------------------
	friend quat nlerp(const quat& q1, const quat& q2, const DATA_TYPE t) { return (lerp(q1, q2, t)).normalized(); }

	friend quat slerp(const quat& q1, const quat& q2, const DATA_TYPE t) {
		DATA_TYPE dt = q1.dot(q2);

		if (abs(dt) >= 1.f) {
			return q1;
		}

		quat q3;
		if (dt < 0.f) {
			dt = -dt;
			q3 = -q2;
		} else {
			q3 = q2;
		}

		if (dt < 0.9999f) {
			const float angle = acos(dt);
			const float invs = 1 / sin(angle);
			const float s0 = sin(angle * (1.f - t));
			const float s1 = sin(angle * t);

			return ((q1 * s0 + q3 * s1) * invs).normalized();
		}

		return nlerp(q1, q3, t);
	}

	friend quat slerpWithSpeed(quat from, quat to, const float speed) {
		float const f = from.dot(to);
		float const angle = acosf(std::fmin(fabsf(f), 1.f)) * 2.f;

		if (angle < 1e-6f) {
			return to;
		}

		float const t = std::fmin(1.f, speed / angle);
		return slerp(from, to, t);
	};

	vec3<DATA_TYPE> transformDir(const vec3<DATA_TYPE>& p) const {
		const quat pResQuat = *this * quat(p, 0.f) * this->conjugate();
		return pResQuat.xyz();
	}

	// Retrieves the rotation created by the quaterion q around the normalized axis normal.
	quat getTwist(const vec3<DATA_TYPE>& normal) const {
		// https://stackoverflow.com/questions/3684269/component-of-a-quaternion-rotation-around-an-axis
		const vec3<DATA_TYPE> ra = this->xyz();
		const vec3<DATA_TYPE> p = ra.dot(normal) * normal;

		quat twist(p.x, p.y, p.z, this->w);
		twist = twist.normalized();

		return twist;
	}

	quat getSwing(const vec3<DATA_TYPE>& normal) const {
		const vec3<DATA_TYPE> ra = this->xyz();
		const vec3<DATA_TYPE> p = ra.dot(normal) * normal;

		quat twist(p.x, p.y, p.z, this->w);
		twist = twist.normalized();

		quat swing = *this * twist.conjugate();

		return swing;
	}

	void getTwistAndSwing(const vec3<DATA_TYPE>& normal, quat& twist, quat& swing) const {
		const vec3<DATA_TYPE> ra = this->xyz();
		const vec3<DATA_TYPE> p = ra.dot(normal) * normal;

		twist = quat(p.x, p.y, p.z, this->w);
		twist = twist.normalized();

		swing = *this * twist.conjugate();
	}


	/// Computes the quaternion which would rotate the vector @fromNormalized to @toNormalized.
	/// @fallbackAxis is used if vectors are pointing in the oposite directions. In that case the rotation will be around that axis.
	/// For @fallbackAxis find any vector that is perpendicular to both specified vectors.
	static quat fromNormalizedVectors(const vec3<DATA_TYPE>& fromNormalized,
	                                  const vec3<DATA_TYPE>& toNormalized,
	                                  const vec3<DATA_TYPE>& fallbackAxis) {
		DATA_TYPE dotProd = clamp(fromNormalized.dot(toNormalized), DATA_TYPE(-1.0), DATA_TYPE(1.0));

		if (isEpsEqual(1.f, dotProd)) {
			return quat::getIdentity();
		} else if (dotProd < DATA_TYPE(-0.99999)) {
			return quat::getAxisAngle(fallbackAxis, pi<DATA_TYPE>());
		} else {
			DATA_TYPE angle = acos(dotProd);
			vec3<DATA_TYPE> axis = fromNormalized.cross(toNormalized).normalized0();
			return quat::getAxisAngle(axis, angle);
		}
	}
};

typedef quat<float> quatf;

} // namespace sge
