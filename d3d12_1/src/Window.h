#pragma once

#include "WinDefines.h"

/* Step 2: Create a class to represent a window. 
* This class will encapsulate the creation and destruction of a window,
* as well as the message-handling. It'll also encapsulate the handle to
* the window, and the operations that work on the handle. 
*/

class Window
{
private:
	/* Step 3: Create a class to handle WNDCLASS - singleton
	* Singleton, b/c we only need one instance of the window class.
	* This class manages the registration/cleanup of the window class.
	* This is the WINAPI version of "class"
	*/ 
	class WindowClass
	{
	public:
		static const wchar_t* GetName() noexcept;								// Getter for getting name of class
		static HINSTANCE GetInstance() noexcept;								// Getter for getting handle to instance
	private:
		WindowClass() noexcept;													// Constructor: registers class on WINAPI side
		~WindowClass();															// Destructor: de-registers class on WINAPI side
		WindowClass(const WindowClass&) = delete;								
		WindowClass& operator=(const WindowClass&) = delete;					
		static constexpr const wchar_t* winClassName = L"D3D12 Engine Window";	// Window class name
		// Only have 1 static instance of WindowClass data type, which will be created when program starts
		static WindowClass winClass;
		HINSTANCE instance;
	};
// Step 4. Define the Window class members
public:
	Window(int width, int height, const wchar_t* name) noexcept;
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
private:
	// Static functions b/c WINAPI doesn't know about C++ features, like member functions. But static does the trick.
	static LRESULT CALLBACK HandleMessageSetup(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMessageThunk(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) noexcept; // Rename?
	LRESULT HandleMessage(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) noexcept;	
private:
	int width;
	int height;
	HWND handle;
};