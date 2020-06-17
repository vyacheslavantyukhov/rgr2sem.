#pragma once

#include "pch.h"
#include "OpenGlWindow.h"
#include "Solver.h"

extern const wchar_t MAIN_CLASS_NAME[];

struct MainWindowState {
	HINSTANCE hInstance = NULL;
	HWND openGlWindow = NULL;
	HWND openButton = NULL;
	HWND fromLabel = NULL;
	HWND fromEdit = NULL;
	HWND toLabel = NULL;
	HWND toEdit = NULL;
	HWND calcButton = NULL;
	HWND ansLabel = NULL;

	Solver solver;
};

HWND CreateMainWindow(HINSTANCE hInstance);

