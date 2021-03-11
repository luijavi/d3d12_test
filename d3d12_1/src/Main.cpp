#include "Window.h"

int WINAPI wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prevInstance, _In_ LPWSTR commandLine, _In_ int showCommand)
{
	try
	{
		Window window(800, 600, L"This is a test window");
		MSG message;
		BOOL g_result;
		while ((g_result = GetMessage(&message, nullptr, 0, 0)) > 0)
		{
			TranslateMessage(&message);
			DispatchMessage(&message);

			// TODO: Delete this test
			while (!window.mouse.IsEmpty())
			{
				const auto e = window.mouse.Read();
				if (e.GetType() == Mouse::Event::Type::Move)
				{
					std::ostringstream oss;
					oss << "Mouse position (" << e.GetXPos() << "," << e.GetYPos() << ")";
					window.SetTitle(oss.str());
				}
			}
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