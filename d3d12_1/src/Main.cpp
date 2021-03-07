#include "Window.h"

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR commandLine, int showCommand)
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