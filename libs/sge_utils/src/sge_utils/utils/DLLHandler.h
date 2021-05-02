#pragma once

namespace sge {

struct DLLHandler {
	DLLHandler() = default;
	~DLLHandler() { unload(); }

	bool load(const char* const path);
	bool loadNoExt(const char* pPath);
	void unload();
	void* getProcAdress(const char* const proc);
	bool isLoaded() { return m_nativeHandle != nullptr; }

  private:
	void* m_nativeHandle = nullptr;
};

} // namespace sge
