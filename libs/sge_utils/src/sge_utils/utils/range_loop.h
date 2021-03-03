#pragma once

#include "sge_utils/sge_utils.h"

namespace sge {

// A for loop helper that iterates in [a, b)
template <typename T>
struct range_t {
	struct const_iterator {
		T i;

		const_iterator(T i)
		    : i(i) {
		}

		bool operator!=(const const_iterator& ref) const {
			return i != ref.i;
		}
		T operator*() const {
			return i;
		}
		const_iterator operator++() {
			++i;
			return *this;
		}
	};

	// a is assumed 0. the interval is [0, _end)
	range_t(const T _end)
	    : _begin(0)
	    , _end(_end) {
	}

	// [_begin, _end)
	range_t(const T _begin, const T _end)
	    : _begin(_begin)
	    , _end(_end) {
		sgeAssert(_begin <= _end);
	}

	const_iterator begin() const {
		return _begin;
	}
	const_iterator end() const {
		return _end;
	}

	T _begin, _end;
};

typedef range_t<unsigned> range_int;
typedef range_t<unsigned> range_u32;
typedef range_t<unsigned> range_size_t;


template <typename T>
struct LoopCArray {
	typedef T value_type;
	typedef value_type* iterator;
	typedef const value_type* const_iterator;

	value_type* const ptr;
	const size_t numElems;

	LoopCArray(value_type* const ptr, const size_t numElems)
	    : ptr(ptr)
	    , numElems(numElems) {
	}

	iterator begin() {
		return ptr;
	}
	iterator end() {
		return ptr + numElems;
	}

	const_iterator begin() const {
		return ptr;
	}
	const_iterator end() const {
		return ptr + numElems;
	}
};

} // namespace sge
