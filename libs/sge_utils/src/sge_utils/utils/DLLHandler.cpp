#if _WIN32
#include "DLLHandler.h"
#include "sge_utils/sge_utils.h"
#include <Windows.h>

namespace sge {

bool DLLHandler::load(const char* const path) {
	unload();
	HMODULE hMod = LoadLibraryA(path);
	static_assert(sizeof(m_nativeHandle) == sizeof(hMod), "DLL Handle size int correct.");
	m_nativeHandle = hMod;

	return hMod != NULL;
}

void DLLHandler::unload() {
	if (m_nativeHandle) {
		[[maybe_unused]] BOOL success = FreeLibrary((HMODULE)m_nativeHandle);
		sgeAssert(success);
	}

	m_nativeHandle = nullptr;
}

void* DLLHandler::getProcAdress(const char* const proc) {
	if (m_nativeHandle == nullptr)
		return nullptr;

	return GetProcAddress((HMODULE)m_nativeHandle, proc);
}

} // namespace sge
#else
#include "DLLHandler.h"

namespace sge {

void DLLHandler::load(const char* const path) {
}

void DLLHandler::unload() {
}

void* DLLHandler::getProcAdress(const char* const proc) {
	return nullptr;
}

} // namespace sge
#endif
