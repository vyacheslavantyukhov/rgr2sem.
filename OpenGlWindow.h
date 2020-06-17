#pragma once

#include "pch.h"

extern const wchar_t OPENGL_CLASS_NAME[];

class Solver;

struct OpenGlWindowState {
	Solver* solver = nullptr;
};


HWND CreateOpenGlWindow(HINSTANCE hInstance, HWND parentWindow, Solver* solver);
