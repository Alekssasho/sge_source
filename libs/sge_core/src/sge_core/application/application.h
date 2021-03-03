#pragma once

#include "sge_core/sgecore_api.h"

#include "sge_utils/sge_utils.h"

#include "input.h"
#include <map>
#include <string>
#include <vector>

int sge_main(int argc, char* argv[]);

namespace sge {

//-----------------------------------------------
// Events and Event data.
//-----------------------------------------------
enum WindowEvent : int {
	WE_Create,
	WE_Destroying,
	WE_Resize,
	WE_FileDrop,
};

struct WE_Resize_Data {
	WE_Resize_Data() = default;

	WE_Resize_Data(int width, int height)
	    : width(width)
	    , height(height) {}

	int width;
	int height;
};

struct WE_FileDrop_Data {
	std::string filename;

};

struct WindowBase;
struct WindowImplData;
struct ApplicationHandlerImplData;

//-----------------------------------------------
// ApplicationHandler
//-----------------------------------------------
struct SGE_CORE_API ApplicationHandler {
  private:
	ApplicationHandler() = default;

  public:
	~ApplicationHandler() = default;

	static ApplicationHandler* get() {
		static ApplicationHandler inst;
		return &inst;
	}

	// Polls and dispatches recieved events to the respective windows.
	void PollEvents();

	template <typename T>
	T* NewWindow(const char* windowName, int width, int height, bool isMaximized, bool noResize = false) {
		T* wnd = new T;
		NewWindowInternal(wnd, windowName, width, height, isMaximized, noResize);
		return wnd;
	}

	void DeregisterWindowInternal(WindowBase* wnd);

	bool HasAliveWindows() const { return m_wnds.size() != 0; }
	const std::vector<WindowBase*>& getAllWindows() { return m_wnds; }

	bool shouldStopRunning() const { return m_isAppQuitRequested || HasAliveWindows() == false; }

  private:
	WindowBase* findWindowBySDLId(const uint32 id);

	void removeWindow(WindowBase* const wndToRemove);

	void NewWindowInternal(WindowBase* window, const char* windowName, int width, int height, bool isMaximized, bool noResize);

  private:
	bool m_isAppQuitRequested = false;
	std::vector<WindowBase*> m_wnds;

	std::map<int, int> sdlJoystickInstanceIdIdToIndex;

	// To be honest having a global input state makes a lot of sense, however,
	// I'm not sure if this is the corrent approch as there is going to be a main window
	// sooo. I'm not going to implement that for now.
	InputState m_inputState;
};

//-----------------------------------------------
// WindowBase
//-----------------------------------------------
struct SGE_CORE_API WindowBase {
	WindowBase();
	virtual ~WindowBase();
	virtual void HandleEvent(const WindowEvent UNUSED(event), const void* const UNUSED(eventData)){};

	WindowBase(WindowBase&) = delete;
	WindowBase& operator=(WindowBase&) = delete;

	void* GetNativeHandle() const;
	bool IsActive() const;
	const InputState& GetInputState() { return m_inputState; }

	int GetClientWidth() const;
	int GetClientHeight() const;

	bool isMaximized() const;

	vec2f getClientSizef() const { return vec2f((float)GetClientWidth(), (float)GetClientHeight()); }

	void setMouseCapture(bool b) { m_shouldCaptureMouse = b; }
	void setMouseRecenterEveryFrame(bool b) { m_shouldRecenterMouseEveryFrame = b; }

	void setWindowTitle(const char* title);

  public:
	InputState m_inputState;

	// true if the window should caputure the mouse and not letting it get outside of the window
	bool m_shouldCaptureMouse = false;

	// true if the window should recenter the mouse.
	bool m_shouldRecenterMouseEveryFrame = false;

	WindowImplData* m_implData;
};

} // namespace sge
