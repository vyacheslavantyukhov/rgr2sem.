#include "pch.h"
#include "MainWindow.h"
#include "OpenGlWindow.h"

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK OpenGLWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void MyRegisterClass(LPCWSTR className, HINSTANCE hInstance, WNDPROC wndProc, UINT style = 0) {
	WNDCLASSEX wcMain = { };

	wcMain.cbSize = sizeof(wcMain);
	wcMain.lpfnWndProc = wndProc;
	wcMain.hInstance = hInstance;
	wcMain.lpszClassName = className;
	wcMain.style = style;

	RegisterClassEx(&wcMain);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	MyRegisterClass(MAIN_CLASS_NAME, hInstance, MainWindowProc);
	MyRegisterClass(OPENGL_CLASS_NAME, hInstance, OpenGLWindowProc, CS_OWNDC);

	HWND hwndMain = CreateMainWindow(hInstance);
	if (hwndMain == NULL)
		return 0;

	ShowWindow(hwndMain, nCmdShow);

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}



