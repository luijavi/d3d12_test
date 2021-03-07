#include "Window.h"

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCommand)
{
	try
	{
		Window window(800, 300, L"This is a test window");
		MSG message;
		BOOL g_result;
		while ((g_result = GetMessage(&message, nullptr, 0, 0)) > 0)
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

		if (g_result == -1)
		{
			return -1;
		}

		return message.wParam;
	}
	catch (const EggCeption& e)
	{
		// if you use a handle instead of nullptr, the window would be modal
		MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBoxA(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}