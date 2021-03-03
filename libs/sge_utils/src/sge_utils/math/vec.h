#pragma once

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "vec_n.h"

namespace sge {

template <typename T, unsigned int N>
struct vec_type_picker {
	typedef vec_n<T, N> type;
};

template <typename T>
struct vec_type_picker<T, 2> {
	typedef vec2<T> type;
};

template <typename T>
struct vec_type_picker<T, 3> {
	typedef vec3<T> type;
};

template <typename T>
struct vec_type_picker<T, 4> {
	typedef vec4<T> type;
};

} // namespace sge
