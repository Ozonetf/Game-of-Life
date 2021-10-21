#include <Windows.h>
#include "Graphics.h"
#include "Main.h"

Graphics* graphics;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	//redraw on window resize
	if (uMsg == WM_PAINT)
	{
		graphics->BeginDraw();
		graphics->ClearScreen(0.0f, 0.0f, .5f);
		for (size_t i = 0; i < 1000; i++)
		{
			graphics->DrawCircle(rand() % 800, rand() % 600, rand() % 300, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f, (rand() % 100) / 100.0f);
		}
		graphics->EndDraw();
	}
	return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hinstance, HINSTANCE prevInstance, LPWSTR cmd, int nCmdShow)
{
	const wchar_t CLASS_NAME[] = L"MainWindow";
	WNDCLASSEX windowclass;
	ZeroMemory(&windowclass, sizeof(WNDCLASSEX));
	windowclass.cbSize = sizeof(WNDCLASSEX);
	windowclass.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
	windowclass.hInstance = hinstance;
	windowclass.lpfnWndProc = WindowProc;
	windowclass.lpszClassName = CLASS_NAME;
	windowclass.style = CS_HREDRAW | CS_VREDRAW;

	RegisterClassEx(&windowclass);

	//rectangle size for the window client without border
	RECT rect = { 0, 0, 800, 600 };
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);

	HWND windowhandle = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW,
		CLASS_NAME,				//window class name
		LPCTSTR("Game of Life"),//window name
		WS_OVERLAPPEDWINDOW,	//style	
		100,					//x coord
		100,					//y coord
		rect.right - rect.left,	//window width
		rect.bottom - rect.top,	//window height
		NULL,					//window parent
		NULL,					//window menu
		hinstance,				//hinstance	
		0);

	if (!windowhandle) return -1;

	graphics = new Graphics();
	if (!graphics->init(windowhandle))
	{
		delete graphics;
		return -1;
	}

	ShowWindow(windowhandle, nCmdShow);
	
	MSG message;
	while (GetMessage(&message, NULL, 0, 0))
	{
		DispatchMessage(&message);
	}

	delete graphics;
	return 0;
}