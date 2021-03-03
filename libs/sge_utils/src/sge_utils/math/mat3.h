#pragma once

#include "math_base.h"
#include "common.h"
#include "quat.h"

namespace sge {

////////////////////////////////////////////////////////////////////////
//struct mat3: A column major storage matrix type
////////////////////////////////////////////////////////////////////////
template<typename TDATA_TYPE>
struct mat3
{
	typedef TDATA_TYPE DATA_TYPE;
	typedef mat3 SELF_TYPE;
	typedef vec3<DATA_TYPE> VEC_TYPE;

	static const unsigned int NUM_ROW = 3;
	static const unsigned int NUM_COL = 3;

	static const unsigned int VEC_SIZE = NUM_ROW;
	static const unsigned int NUM_VECS = NUM_COL;

	//keep this named like this in order to be abe to quickly add row major
	VEC_TYPE data[NUM_VECS];

	#include "matrix_common.inl"

	//---------------------------------------------------
	//inverse
	//---------------------------------------------------
	SELF_TYPE inverse() const
	{
		SELF_TYPE result;

		const DATA_TYPE invDet = (DATA_TYPE)1.0 / triple(data[0], data[1], data[2]);

		result.data[0][0] =  (data[1][1]*data[2][2] - data[2][1]*data[1][2]) * invDet;
		result.data[0][1] = -(data[0][1]*data[2][2] - data[2][1]*data[0][2]) * invDet;
		result.data[0][2] =  (data[0][1]*data[1][2] - data[0][2]*data[1][1]) * invDet;

		result.data[1][0] = -(data[1][0]*data[2][2] - data[1][2]*data[2][0]) * invDet;
		result.data[1][1] =  (data[0][0]*data[2][2] - data[0][2]*data[2][0]) * invDet;
		result.data[1][2] = -(data[0][0]*data[1][2] - data[0][2]*data[1][0]) * invDet;

		result.data[2][0] =  (data[1][0]*data[2][1] - data[1][1]*data[2][0]) * invDet;
		result.data[2][1] = -(data[0][0]*data[2][1] - data[0][1]*data[2][0]) * invDet;
		result.data[2][2] =  (data[0][0]*data[1][1] - data[1][0]*data[0][1]) * invDet;

		return result;
	}

	friend SELF_TYPE inverse(const SELF_TYPE& m)
	{
		return m.Inverse();
	}

	////////////////////////////////////////////////////////////////////////
	//Commonly used transformations
	////////////////////////////////////////////////////////////////////////

	//----------------------------------------------------------------
	//2d translation
	//----------------------------------------------------------------
	static SELF_TYPE getTranslation(const DATA_TYPE& tx, const DATA_TYPE& ty)
	{
		SELF_TYPE result;

		result.data[0] = VEC_TYPE::getAxis(0);
		result.data[1] = VEC_TYPE::getAxis(1);
		result.data[2] = VEC_TYPE(tx, ty, (DATA_TYPE)1.0);

		return result;
	}

	static SELF_TYPE getTranslation(const vec2<DATA_TYPE>& t)
	{
		SELF_TYPE result;

		result.data[0] = VEC_TYPE::getAxis(0);
		result.data[1] = VEC_TYPE::getAxis(1);
		result.data[2] = VEC_TYPE(t.x, t.y, (DATA_TYPE)1.0);

		return result;
	}

	//----------------------------------------------------------------
	//scaling
	//----------------------------------------------------------------
	static SELF_TYPE getScaling(const DATA_TYPE& sx, const DATA_TYPE& sy, const DATA_TYPE& sz)
	{
		SELF_TYPE result;

		result.data[0] = VEC_TYPE::getAxis(0, sx);
		result.data[1] = VEC_TYPE::getAxis(1, sy);
		result.data[2] = VEC_TYPE::getAxis(2, sz);

		return result;
	}

	//----------------------------------------------------------------
	//X rotation matrix
	//----------------------------------------------------------------
	static SELF_TYPE getRotationX(const DATA_TYPE& angle)
	{
		SELF_TYPE result;

		DATA_TYPE s,c; SinCos(angle, s, c);

		result.data[0] = VEC_TYPE::getAxis(0);
		result.data[1] = VEC_TYPE(0,  c, s);
		result.data[2] = VEC_TYPE(0, -s, c);

		return result;
	}

	//----------------------------------------------------------------
	//Y rotation matrix
	//----------------------------------------------------------------
	static SELF_TYPE getRotationY(const DATA_TYPE& angle)
	{
		SELF_TYPE result;

		DATA_TYPE s,c; SinCos(angle, s, c);

		result.data[0] = VEC_TYPE(c, 0, -s);
		result.data[1] = VEC_TYPE::getAxis(1);
		result.data[2] = VEC_TYPE(s, 0,  c);

		return result;
	}

	//----------------------------------------------------------------
	//Z rotation matrix
	//----------------------------------------------------------------
	static SELF_TYPE getRotationZ(const DATA_TYPE& angle)
	{
		SELF_TYPE result;

		DATA_TYPE s,c; SinCos(s, c, angle);

		result.data[0] = VEC_TYPE( c, s, 0);
		result.data[1] = VEC_TYPE(-s, c, 0);
		result.data[2] = VEC_TYPE::getAxis(2);

		return result;
	}

	//----------------------------------------------------------------
	//Quaternion rotation
	//----------------------------------------------------------------
	static SELF_TYPE getRotationQuat(const quat<DATA_TYPE>& q)
	{
		SELF_TYPE result;

		const DATA_TYPE& x = q[0];
		const DATA_TYPE& y = q[1];
		const DATA_TYPE& z = q[2];
		const DATA_TYPE& w = q[3];

		const DATA_TYPE one = (DATA_TYPE)1.0;
		const DATA_TYPE two = (DATA_TYPE)2.0;

		result.data[0] = VEC_TYPE(	one - two*(y*y + z*z),
									two*(x*y + z*w),
									two*(x*z - y*w));

		result.data[1] = VEC_TYPE(	two*(x*y - z*w),
									one - two*(x*x + z*z),
									two*(y*z + x*w));

		result.data[2] = VEC_TYPE(	two*(x*z + y*w),
									two*(y*z - x*w),
									one - two*(x*x + y*y));

		return result;
	}
};

typedef  mat3<float> mat3f;

}
