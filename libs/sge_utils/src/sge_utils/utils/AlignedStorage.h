#pragma once

#include  <cstddef>

namespace sge {

template <std::size_t Len, std::size_t Align /* default alignment not implemented */>
struct AlignedStorage {
	struct type {
		alignas(Align) unsigned char data[Len];
	};
};

} // namespace sge