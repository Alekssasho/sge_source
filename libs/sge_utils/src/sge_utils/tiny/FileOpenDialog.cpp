#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <Windows.h>
	#include <commdlg.h>
	#include <mutex>
#else
	#include <cstdio>
#endif

#include "FileOpenDialog.h"

namespace sge { 

bool DialogYesNo(const char* caption, const char* message)
{
#ifdef WIN32
	int res = MessageBoxA(NULL, message, caption, MB_YESNO | MB_ICONQUESTION);
	return res == IDYES;
#else
	// TODO: Question text. It is done with --text="my-text". Careful with "!" sign.
	int res = system("zenity --question");
	return res == 0; // apperantly 0 is yes.
#endif
}

std::string FileOpenDialog(const std::string& prompt, bool fileMustExists, const char* fileFilter)
{
#ifdef WIN32

	static std::mutex mtx;
	std::lock_guard<std::mutex> mtx_guard(mtx);

	const int BUFSIZE = 1024;
	char buffer[BUFSIZE] = {0};

	TCHAR currentDir[MAX_PATH];

	// GetOpenFileName changes the current directory. We do not want that so we revert it back to what it was.
	if(GetCurrentDirectory(ARRAYSIZE(currentDir), currentDir)!=0)
	{
		OPENFILENAME ofns = {0};
		ofns.lStructSize = sizeof(ofns);
		ofns.lpstrFile = buffer;
		ofns.nMaxFile = BUFSIZE;
		ofns.lpstrTitle = prompt.c_str();
		ofns.lpstrFilter = fileFilter ? fileFilter : "All Files\0*.*\0";
		
		if(fileMustExists)
		{
			ofns.Flags |= OFN_FILEMUSTEXIST;
		}

		const BOOL okClicked  = GetOpenFileNameA(&ofns);

		SetCurrentDirectory(currentDir);
	}
	else
	{
		sgeAssert(false);
	}

	return std::string(buffer);
#else 
	#if !defined(__EMSCRIPTEN__)
		// [TODO] Fix this madness...
		char file[1024] = {0};
		FILE* const f = popen("zenity --file-selection", "r");
		sgeAssert(f!=nullptr);
		fgets(file, 1024, f);
		file[SGE_ARRSZ(file)] = '\0'; // clamp if the filename is too long.
		std::string s(file); // [TODO] Zenitiy.
		s.pop_back(); // Delete the '\n' printed by zenity.
		pclose(f);
		return s;
	#else
		return std::string();
	#endif
#endif
}

std::string FileSaveDialog(const std::string& prompt, const char* fileFilter, const char* defaultExtension)
{
#ifdef WIN32

	static std::mutex mtx;
	std::lock_guard<std::mutex> mtx_guard(mtx);

	const int BUFSIZE = 1024;
	char buffer[BUFSIZE] = {0};

	TCHAR currentDir[MAX_PATH];

	// GetOpenFileName changes the current directory. We do not want that so we revert it back to what it was.
	if(GetCurrentDirectory(ARRAYSIZE(currentDir), currentDir)!=0)
	{
		OPENFILENAME ofns = {0};
		ofns.lStructSize = sizeof(ofns);
		ofns.lpstrFile = buffer;
		ofns.nMaxFile = BUFSIZE;
		ofns.lpstrTitle = prompt.c_str();
		ofns.lpstrDefExt = defaultExtension;
		ofns.lpstrFilter = fileFilter ? fileFilter : "All Files\0*.*\0";
		ofns.Flags |= OFN_FILEMUSTEXIST;
		
		const BOOL okClicked  = GetSaveFileNameA(&ofns);

		SetCurrentDirectory(currentDir);
	}
	else
	{
		sgeAssert(false);
	}

	return std::string(buffer);
#else
	// Implement.
	sgeAssert(false);
	return std::string();
#endif
}

}
