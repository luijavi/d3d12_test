#include "Window.h"
#include <sstream>
#include "resource.h"

// Window class stuff
Window::WindowClass Window::WindowClass::winClass;

const wchar_t* Window::WindowClass::GetName() noexcept
{
	return winClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return winClass.instance;
}

// Constructor
Window::WindowClass::WindowClass() noexcept
	:
	instance(GetModuleHandle(nullptr)) // So that we don't have to take a param in constructor
{
	// Fill out WNDCLASS structure
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMessageSetup;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetInstance();
	wc.hIcon = static_cast<HICON>(LoadImage(instance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, 0));
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = static_cast<HICON>(LoadImage(instance, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, 0));
	RegisterClassEx(&wc);	// Register window class
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(winClassName, GetInstance());
}

// Window creation and handling stuff
Window::Window(int width, int height, const wchar_t* name) noexcept
	:
	width(width),
	height(height)
{
	// Calculate window size based on desired client region size.
	RECT winRect;
	winRect.left = 100;
	winRect.right = width + winRect.left;
	winRect.top = 100;
	winRect.bottom = height + winRect.top;
	// This is so that you could change the size of the client region, while taking into account
	// the rest of the window.
	if (AdjustWindowRect(&winRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
	{
		throw EGGCEPT_LAST_EXCEPT();	// Check if AdjustWindow fails
	}
	// Create window & get handle
	handle = CreateWindow(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		winRect.right - winRect.left,
		winRect.bottom - winRect.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);
	// check if CreateWindow succeeds
	if (handle == nullptr)
	{
		throw EGGCEPT_LAST_EXCEPT();
	}
	// Show window
	ShowWindow(handle, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(handle);
}

void Window::SetTitle(const std::string& title)
{
	if (SetWindowTextA(handle, title.c_str()) == 0)
	{
		throw EGGCEPT_LAST_EXCEPT();
	}
}

// This function is mainly to install/set up a pointer to our instance in the Win32 side
LRESULT WINAPI Window::HandleMessageSetup(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
	// Basically, it'll just see if the message being handled is WM_NCCREATE
	// Use create parameter passed in from CreateWindow() (aka "this" in the CreateWindow() )
	// to store a Window Class ptr at WinAPI
	if (message == WM_NCCREATE)
	{
		// Extract ptr to Window Class from creation data
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWindow = static_cast<Window*>(pCreate->lpCreateParams);
		// Set WinAPI-managed user data to store ptr to window class
		SetWindowLongPtr(handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
		// Set message procedure to normal (non-setup) handler now that the setup is finished
		SetWindowLongPtr(handle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMessageThunk));
		// Forward messages to window class handler
		return pWindow->HandleMessage(handle, message, wParam, lParam);
	}
	// If we get a message before NCCREATE, handle w/ default window proc
	return DefWindowProc(handle, message, wParam, lParam);
}

// This just adapts from the Win32 call convention to the C++ member function call convention
LRESULT WINAPI Window::HandleMessageThunk(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
	// Retrieve pointer to our Window Class from the data stored in the Win32 side
	Window* const pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(handle, GWLP_USERDATA));
	// Forward message to window class handler
	return pWindow->HandleMessage(handle, message, wParam, lParam);
}

LRESULT Window::HandleMessage(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
	// Main switch statement to handle messages
	switch (message)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		} break;
		case WM_KILLFOCUS:
		{
			// clear key state when window loses focus to prevent input from getting stuck
			kbd.ClearState();
		} break;
		/************ KEYBOARD MESSAGES ************/
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:	// Syskeys need to be handled to track ALT key (VK_MENU)
		{
			if (!(lParam & 0x40000000) || kbd.AutoRepeatIsEnabled()) // 0x40000000 is the same as 2^30, to rep the 30th bit
			{
				kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
			}
		} break;
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		} break;
		case WM_CHAR:
		{
			kbd.OnChar(static_cast<unsigned char>(wParam));
		} break;
		/********** END KEYBOARD MESSAGES **********/
		/************* MOUSE MESSAGES **************/
		case WM_MOUSEMOVE:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			// if mouse moved in client region -> log move, and log enter + capture mouse (if not previously)
			if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
			{
				mouse.OnMouseMove(pt.x, pt.y);
				if (!mouse.IsInWindow())
				{
					SetCapture(handle);		// windows API function that captures mouse
					mouse.OnMouseEnter();
				}
				// not in client -> log move / maintain capture if button down
				else
				{
					if (wParam & (MK_LBUTTON | MK_RBUTTON))
					{
						mouse.OnMouseMove(pt.x, pt.y);	// generate a mouse move, even if outside of client region
					}
					// button up -> release capture / log event for leaving
					else
					{
						ReleaseCapture();
						mouse.OnMouseLeave();
					}

				}
			}
		} break;
		case WM_LBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftPressed(pt.x, pt.y);
		} break;
		case WM_RBUTTONDOWN:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightPressed(pt.x, pt.y);
		} break;
		case WM_LBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnLeftReleased(pt.x, pt.y);
		} break;
		case WM_RBUTTONUP:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			mouse.OnRightReleased(pt.x, pt.y);
		} break;
		case WM_MOUSEWHEEL:
		{
			const POINTS pt = MAKEPOINTS(lParam);
			if (GET_WHEEL_DELTA_WPARAM(wParam) > 0)
			{
				mouse.OnWheelUp(pt.x, pt.y);
			}
			else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0)
			{
				mouse.OnWheelDown(pt.x, pt.y);
			}
		} break;
		/*********** END MOUSE MESSAGES ************/
	}

	return DefWindowProc(handle, message, wParam, lParam);
}

// Window Exceptions
Window::Exception::Exception(int line, const char* file, HRESULT hResult) noexcept
	:
	EggCeption(line, file),
	hResult(hResult)
{}

const char* Window::Exception::what() const noexcept
{
	std::ostringstream strStream;
	strStream << GetType() << std::endl
			  << "[Error Code] " << GetErrorCode() << std::endl
			  << "[Description] " << GetErrorString() << std::endl
			  << GetOriginString();
	whatBuffer = strStream.str();
	return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept
{
	return "EggCeption: Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hResult) noexcept
{
	char* pMessageBuf = nullptr;
	// Calls Window function that takes HRESULT and returns a string for
	// that error code. The return value is the length of the error msg
	DWORD nMessageLen = FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, hResult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&pMessageBuf), 0, nullptr
	);

	if (nMessageLen == 0)
	{
		return "Unidentified error code";
	}
	// take error code that's in the windows allocated buffer and copy it
	// into a std::string, then free that memory, and then return the std:string
	std::string errorString = pMessageBuf;
	LocalFree(pMessageBuf);
	return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
	return hResult;
}

std::string Window::Exception::GetErrorString() const noexcept
{
	return TranslateErrorCode(hResult);
}
