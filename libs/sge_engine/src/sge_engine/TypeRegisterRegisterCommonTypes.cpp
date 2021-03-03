#include "sge_utils/math/Box.h"
#include "sge_utils/math/Rangef.h"
#include "sge_utils/math/mat4.h"
#include "sge_utils/math/transform.h"

#include "sge_utils/math/MultiCurve2D.h"

#include <string>
#include <vector>

#include "TypeRegister.h"

namespace sge {

// clang-format off
DefineTypeId(bool,                       1);
DefineTypeId(char,                       2);
DefineTypeId(int,                        3);
DefineTypeId(unsigned,                   4);
DefineTypeId(float,                      5);
DefineTypeId(double,                     6);

//DefineTypeId(std::vector<bool>,        7);
DefineTypeId(std::vector<char>,          8);
DefineTypeId(std::vector<int>,           9);
DefineTypeId(std::vector<unsigned>,     10);
DefineTypeId(std::vector<float>,        11);
DefineTypeId(std::vector<double>,       12);

DefineTypeId(std::string,               13);

DefineTypeId(vec2i,                     14);
DefineTypeId(vec3i,                     15);
DefineTypeId(vec4i,                     16);

DefineTypeId(vec2f,                     17);
DefineTypeId(vec3f,                     18);
DefineTypeId(vec4f,                     19);
DefineTypeId(quatf,                     20);

DefineTypeId(mat4f,                     21);
DefineTypeId(transf3d,                  22);

DefineTypeId(AABox3f,                   23);

DefineTypeId(Rangef,                    24);

DefineTypeId(MultiCurve2D::PointType,                25);
DefineTypeId(MultiCurve2D::Point,                    26);
DefineTypeId(std::vector<MultiCurve2D::Point>,       27);
DefineTypeId(MultiCurve2D,                           28);

DefineTypeId(std::vector<vec2i>,                     29);
DefineTypeId(std::vector<vec3i>,                     30);
DefineTypeId(std::vector<vec4i>,                     31);

DefineTypeId(std::vector<vec2f>,                     32);
DefineTypeId(std::vector<vec3f>,                     33);
DefineTypeId(std::vector<vec4f>,                     34);
DefineTypeId(std::vector<quatf>,                     35);

DefineTypeId(Axis,                                   40);

DefineTypeId(std::vector<transf3d>,                  41);

DefineTypeId(short,                                  42);
DefineTypeId(unsigned short,                         43);

DefineTypeId(TypeId,                                 44);
DefineTypeId(std::vector<TypeId>,                    45);

ReflBlock() {
	ReflAddType(bool);
	ReflAddType(char);
	ReflAddType(int);
	ReflAddType(unsigned);
	ReflAddType(float);
	ReflAddType(double);

	//ReflAddType(std::vector<bool>);
	ReflAddType(std::vector<char>);
	ReflAddType(std::vector<int>);
	ReflAddType(std::vector<unsigned>);
	ReflAddType(std::vector<float>);
	ReflAddType(std::vector<double>);

	ReflAddType(std::string);

	// vec*i
	ReflAddType(vec2i)
		ReflMember(vec2i, x)
		ReflMember(vec2i, y);
	ReflAddType(vec3i)
		ReflMember(vec3i, x)
		ReflMember(vec3i, y)
		ReflMember(vec3i, z);
	ReflAddType(vec4i)
		ReflMember(vec4i, x)
		ReflMember(vec4i, y)
		ReflMember(vec4i, z)
		ReflMember(vec4i, w);

	// vec*f
	ReflAddType(vec2f)
		ReflMember(vec2f, x)
		ReflMember(vec2f, y);
	ReflAddType(vec3f)
		ReflMember(vec3f, x)
		ReflMember(vec3f, y)
		ReflMember(vec3f, z);
	ReflAddType(vec4f)
		ReflMember(vec4f, x)
		ReflMember(vec4f, y)
		ReflMember(vec4f, z)
		ReflMember(vec4f, w);

	ReflAddType(quatf)
		ReflMember(quatf, x)
		ReflMember(quatf, y)
		ReflMember(quatf, z)
		ReflMember(quatf, w);
	
	ReflAddType(std::vector<vec2i>);
	ReflAddType(std::vector<vec3i>);
	ReflAddType(std::vector<vec4i>);
	
	ReflAddType(std::vector<vec2f>);
	ReflAddType(std::vector<vec3f>);
	ReflAddType(std::vector<vec4f>);
	ReflAddType(std::vector<quatf>);

	ReflAddType(mat4f)
		ReflMember(mat4f, c0)
		ReflMember(mat4f, c1)
		ReflMember(mat4f, c2)
		ReflMember(mat4f, c3);

	ReflAddType(transf3d)
		ReflMember(transf3d, p).setPrettyName("position")
		ReflMember(transf3d, r).setPrettyName("rotation")
		ReflMember(transf3d, s).setPrettyName("scaling")
	;

	ReflAddType(AABox3f)
		ReflMember(AABox3f, min)
		ReflMember(AABox3f, max);

	ReflAddType(Rangef)
		ReflMember(Rangef, min)
		ReflMember(Rangef, max);

		// MultiCurve2D
	ReflAddType(MultiCurve2D::PointType)
		ReflEnumVal(MultiCurve2D::pointType_linear, "Linear")
		ReflEnumVal(MultiCurve2D::pointType_constant, "Constant")
		ReflEnumVal(MultiCurve2D::pointType_smooth, "Smooth")
		ReflEnumVal(MultiCurve2D::pointType_bezierKey, "Bezier")
		ReflEnumVal(MultiCurve2D::pointType_bezierHandle0, "BezierHandle0")
		ReflEnumVal(MultiCurve2D::pointType_bezierHandle1, "BezierHandle1")
	;

	ReflAddType(MultiCurve2D::Point)
		ReflMember(MultiCurve2D::Point, type)
		ReflMember(MultiCurve2D::Point, x)
		ReflMember(MultiCurve2D::Point, y)
	;

	ReflAddType(std::vector<MultiCurve2D::Point>);

	ReflAddType(MultiCurve2D)
		ReflMember(MultiCurve2D, m_pointsWs)
	;

	ReflAddType(Axis)
		ReflEnumVal(axis_x, "axisIdx_x")
		ReflEnumVal(axis_y, "axisIdx_y")
		ReflEnumVal(axis_z, "axisIdx_z")
	;

	ReflAddType(std::vector<transf3d>);

	ReflAddType(short);
	ReflAddType(unsigned short);

	ReflAddType(TypeId)
		ReflMember(TypeId, id);

	ReflAddType(std::vector<TypeId>);
}

// clang-format on

} // namespace sge
