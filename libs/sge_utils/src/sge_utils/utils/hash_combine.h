#pragma once

namespace sge {

template <typename T>
inline T hash_combine(T seed, T value) {
	return seed ^ (value + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

} // namespace sge
