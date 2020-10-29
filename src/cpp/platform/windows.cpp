#include <windows.h>
#include <string>
#include <system_error>

#define DISPLAY_WINDOW_CLASS "ObsNodeDisplayWindow"

bool displayWindowClassRegistered = false;
WNDCLASSEX DisplayWndClassObj;
ATOM DisplayWndClassAtom;

void throwLastErrorMessage() {
    DWORD errorCode = GetLastError();
    LPSTR errorStr = nullptr;
    DWORD errorStrSize = 16;
    DWORD errorStrLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                                      NULL, errorCode, LANG_USER_DEFAULT, errorStr, errorStrSize, NULL);
    std::string exceptionMessage(errorStr, errorStrLen);
    exceptionMessage = "Unexpected WinAPI error: " + exceptionMessage;
    LocalFree(errorStr);
    throw std::system_error(errorCode, std::system_category(), exceptionMessage);
}

LRESULT CALLBACK DisplayWndProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
    // skip hit test for display window
    switch (uMsg) {
        case WM_NCHITTEST:
            return HTTRANSPARENT;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void registerDisplayWindowClass() {
    if (displayWindowClassRegistered) {
        return;
    }
    displayWindowClassRegistered = true;
    DisplayWndClassObj.cbSize = sizeof(WNDCLASSEX);
    DisplayWndClassObj.style  = CS_OWNDC | CS_NOCLOSE | CS_HREDRAW | CS_VREDRAW; // CS_DBLCLKS | CS_HREDRAW | CS_NOCLOSE | CS_VREDRAW | CS_OWNDC;
    DisplayWndClassObj.lpfnWndProc = DisplayWndProc;
    DisplayWndClassObj.cbClsExtra = 0;
    DisplayWndClassObj.cbWndExtra = 0;
    DisplayWndClassObj.hInstance = NULL; // HINST_THISCOMPONENT;
    DisplayWndClassObj.hIcon = NULL;
    DisplayWndClassObj.hCursor = NULL;
    DisplayWndClassObj.hbrBackground = NULL;
    DisplayWndClassObj.lpszMenuName = NULL;
    DisplayWndClassObj.lpszClassName = DISPLAY_WINDOW_CLASS;
    DisplayWndClassObj.hIconSm = NULL;
    DisplayWndClassAtom = RegisterClassEx(&DisplayWndClassObj);
    if (DisplayWndClassAtom == NULL) {
        throwLastErrorMessage();
    }
}

void *createDisplayWindow(void *parentHandle) {
    HWND parentHandleHWND = *static_cast<HWND*>(parentHandle);
    registerDisplayWindowClass();
    HWND windowHandle = CreateWindowEx(
            0, // WS_EX_COMPOSITED | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST,
            DISPLAY_WINDOW_CLASS,
            "Display Window",
            WS_VISIBLE | WS_POPUP,
            0, 0, 0, 0,
            parentHandleHWND,
            NULL,
            NULL,
            NULL);
    if (windowHandle == NULL) {
        throwLastErrorMessage();
    }
    SetParent(windowHandle, parentHandleHWND);
    return reinterpret_cast<void*>(windowHandle);
}

void destroyWindow(void *windowHandle) {
    HWND handle = reinterpret_cast<HWND>(windowHandle);
    DestroyWindow(handle);
}

void moveWindow(void *windowHandle, int x, int y, int width, int height) {
    HWND handle = reinterpret_cast<HWND>(windowHandle);
    BOOL result = SetWindowPos(
            handle,
            NULL,
            x,
            y,
            width,
            height,
            SWP_NOCOPYBITS | SWP_NOACTIVATE | SWP_NOZORDER | SWP_SHOWWINDOW
    );
    if(result) {
        RedrawWindow( handle, NULL, NULL, RDW_ERASE | RDW_INVALIDATE);
    }
}