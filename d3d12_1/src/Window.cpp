#include "Window.h"

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
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);	// Register window class
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass(winClassName, GetInstance());
}

// Window creation and handling stuff
Window::Window(int width, int height, const wchar_t* name) noexcept
{
	// Calculate window size based on desired client region size.
	RECT winRect;
	winRect.left = 100;
	winRect.right = width + winRect.left;
	winRect.top = 100;
	winRect.bottom = height + winRect.top;
	// This is so that you could change the size of the client region, while taking into account
	// the rest of the window.
	AdjustWindowRect(&winRect, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);
	// Create window & get handle
	handle = CreateWindow(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		winRect.right - winRect.left,
		winRect.bottom - winRect.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);
	// Show window
	ShowWindow(handle, SW_SHOWDEFAULT);
}

Window::~Window()
{
	DestroyWindow(handle);
}

// This function is mainly to install/set up a pointer to our instance in the Win32 side
LRESULT WINAPI Window::HandleMessageSetup(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
	// Basically, it'll just see if the message being handled is WM_NCCREATE
	// Use create parameter passed in from CreateWindow() (aka "this" in the CreateWindow() )
	// to store a Window Class ptr at WinAPI
	if (message = WM_NCCREATE)
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
	switch (message)
	{
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		} break;
	}

	return DefWindowProc(handle, message, wParam, lParam);
}
