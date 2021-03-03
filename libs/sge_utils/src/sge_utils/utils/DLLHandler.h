#pragma once

namespace sge {

struct DLLHandler {
	bool load(const char* const path);
	void unload();
	void* getProcAdress(const char* const proc);
	bool isLoaded() { return m_nativeHandle != nullptr; }

  private:
	void* m_nativeHandle = nullptr;
};

} // namespace sge
