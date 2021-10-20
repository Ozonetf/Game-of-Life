#include <Windows.h>
#include <d2d1_1.h>
#include "Main.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_DESTROY)
	{
		PostQuitMessage(0);
		return 0;
	}
	DefWindowProcA(hwnd, uMsg, wParam, lParam);
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
	HWND windowhandle = CreateWindow(
		CLASS_NAME,
		LPCTSTR("Game of Life"),
		WS_OVERLAPPEDWINDOW,
		100,
		100,
		800,
		600,
		NULL,
		NULL,
		hinstance,
		0);

	if (!windowhandle) return -1;

	ShowWindow(windowhandle, nCmdShow);
	
	MSG message;
	while (GetMessage(&message, NULL, 0, 0))
	{
		DispatchMessage(&message);
	}
	return 0;
}