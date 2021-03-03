#pragma once

#include <functional>

#include "basetypes.h"

namespace sge {

struct ScopeGuard : public Noncopyable {
	ScopeGuard() = default;
	ScopeGuard(std::function<void()> scopeExitProc)
	    : scopeExitProc(std::move(scopeExitProc)) {
	}

	~ScopeGuard() {
		if (scopeExitProc) {
			scopeExitProc();
		}
	}

	void dismiss() {
		scopeExitProc = nullptr;
	}

  private:
	std::function<void()> scopeExitProc;
};
} // namespace sge
