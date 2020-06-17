#include "pch.h"
#include "MainWindow.h"
#include <windowsx.h>
#include <string>

const wchar_t MAIN_CLASS_NAME[] = L"MainWindowClass";

#define IDB_Open_Button 1
#define IDB_From_Edit 2
#define IDB_To_Edit 3
#define IDB_Solve_Button 4
#define IDB_Answer 5

MainWindowState* OnMainWindowCreate(HWND hwnd, LPARAM lParam);
LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
std::wstring GetOpenFileName(HWND hwnd, HINSTANCE hInstance);

HWND CreateMainWindow(HINSTANCE hInstance) {
	MainWindowState* state = new MainWindowState();

	HWND window = CreateWindowEx(
		0,                              // Optional window styles.
		MAIN_CLASS_NAME,                     // Window class
		L"Network",    // Window text
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		state		// Additional application data
	);
	return window;
}

MainWindowState* OnMainWindowCreate(HWND hwnd, LPARAM lParam) {
	CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
	MainWindowState* pState = reinterpret_cast<MainWindowState*>(pCreate->lpCreateParams);
	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pState);

	pState->hInstance = pCreate->hInstance;

	pState->openGlWindow = CreateOpenGlWindow(pCreate->hInstance, hwnd, &pState->solver);

	int defControlPos = 570;
	int defControlWidth = 200;
	int defControlHeight = 20;

	pState->openButton = CreateWindowEx(
		0,
		L"BUTTON",
		L"Загрузить данные",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		defControlPos, 500, defControlWidth, defControlHeight,
		hwnd,
		(HMENU) IDB_Open_Button,
		pState->hInstance,
		NULL
	);

	pState->fromLabel = CreateWindowEx(
		0,
		L"STATIC",
		L"Точка A:",
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		defControlPos, 40, defControlWidth, defControlHeight,
		hwnd,
		NULL,
		pState->hInstance,
		NULL
	);

	pState->fromEdit = CreateWindowEx(
		0,
		L"EDIT",
		L"0",
		WS_CHILD | WS_VISIBLE | ES_LEFT,
		defControlPos, 70, defControlWidth, defControlHeight,
		hwnd,
		(HMENU)IDB_From_Edit,
		pState->hInstance,
		NULL
	);

	pState->toLabel = CreateWindowEx(
		0,
		L"STATIC",
		L"Точка B:",
		WS_VISIBLE | WS_CHILD | SS_CENTER,
		defControlPos, 120, defControlWidth, defControlHeight,
		hwnd,
		NULL,
		pState->hInstance,
		NULL
	);

	pState->toEdit = CreateWindowEx(
		0,
		L"EDIT",
		L"0",
		WS_CHILD | WS_VISIBLE | ES_LEFT,
		defControlPos, 150, defControlWidth, defControlHeight,
		hwnd,
		(HMENU)IDB_From_Edit,
		pState->hInstance,
		NULL
	);

	pState->calcButton = CreateWindowEx(
		0,
		L"BUTTON",
		L"Вычислить",
		WS_CHILD | WS_VISIBLE,
		defControlPos, 190, defControlWidth, defControlHeight,
		hwnd,
		(HMENU)IDB_Solve_Button,
		pState->hInstance,
		NULL
	);

	pState->ansLabel = CreateWindowEx(
		0,
		L"STATIC",
		L"Загрузите данные",
		WS_CHILD | WS_VISIBLE | SS_LEFT,
		defControlPos, 240, defControlWidth, defControlHeight * 3,
		hwnd,
		(HMENU)IDB_Answer,
		pState->hInstance,
		NULL
	);

	return pState;
}

int CheckAndGetEdit(HWND edit, int maxVal) {
	static wchar_t buff[1024];
	GetWindowText(edit, buff, sizeof(buff));
	int val;
	try {
		val = _wtoi(buff);
	}
	catch (std::exception& e) {
		val = 0;
	}
	val = min(val, maxVal);
	val = max(val, 0);
	_itow(val, buff, 10);
	SetWindowText(edit, buff);
	UpdateWindow(edit);
	return val;
}

LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	MainWindowState* pState;
	if (uMsg == WM_CREATE)
	{
		pState = OnMainWindowCreate(hwnd, lParam);
		SendMessage(hwnd, WM_SIZE, 0, 0);
		return 0;
	}
	else {
		pState = reinterpret_cast<MainWindowState*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW));
		EndPaint(hwnd, &ps);
	}
	return 0;
	case WM_SETCURSOR:
		if (LOWORD(lParam) == HTCLIENT)
		{
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			return TRUE;
		}
		break;
	case WM_SIZE:
	{
		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		InvalidateRect(hwnd, &clientRect, TRUE);
		UpdateWindow(hwnd);
	}
	return 0;
	case WM_COMMAND:
	{
		if (wParam == IDB_Open_Button) {
			std::wstring fileName = GetOpenFileName(hwnd, pState->hInstance);
			if (!fileName.empty()) {
				if (pState->solver.Load(fileName)) {
					UpdateWindow(pState->openGlWindow);
					Static_SetText(pState->ansLabel, L"Данные загружены. Введите две отличные вершины");
				}
				else {
					pState->solver.Clear();
					Static_SetText(pState->ansLabel, L"Ошибка чтения файла или формата данных");
				}
			}
			return 0;
		}
		else if (wParam == IDB_Solve_Button) {
			if (pState->solver.nOrig > 0) {
				int source = CheckAndGetEdit(pState->fromEdit, pState->solver.nOrig - 1);
				int sink = CheckAndGetEdit(pState->toEdit, pState->solver.nOrig - 1);
				if (source == sink) {
					pState->solver.source = pState->solver.sink = -1;
					Static_SetText(pState->ansLabel, L"Вершины должны быть отличны");
				}
				else {
					int ans = pState->solver.GetAnswer(source, sink);
					if (ans == -1) {
						Static_SetText(pState->ansLabel, L"Вершины смежны");
					}
					else {
						wchar_t buff[256];
						wsprintf(buff, L"Ответ: %d", ans);
						Static_SetText(pState->ansLabel, buff);
						UpdateWindow(pState->openGlWindow);
					}
				}
				
			}
			else {
				Static_SetText(pState->ansLabel, L"Граф не загружен или пустой");
			}
		}
	}
	break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

std::wstring GetOpenFileName(HWND hwnd, HINSTANCE hInstance)
{
	wchar_t buffer[1024];
	buffer[0] = 0;
	OPENFILENAME ofn = {
		sizeof(OPENFILENAME),
		hwnd,
		hInstance,
		L"Text Files (*.txt)\0*.txt\0\0",
		NULL,
		0,
		0,
		buffer,
		sizeof(buffer),
		NULL,
		NULL,
		0,
		0,
		0,
		NULL,
		NULL,
		NULL,
		NULL
	};

	if (GetOpenFileName(&ofn)) {
		return ofn.lpstrFile;
	}
	else {
		return L"";
	}
}