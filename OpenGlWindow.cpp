#include "pch.h"
#include "OpenGlWindow.h"
#include "Solver.h"

const wchar_t OPENGL_CLASS_NAME[] = L"OpenGLWindowClass";

void DrawCircle(GLfloat x, GLfloat y, GLfloat radius, GLfloat r, GLfloat g, GLfloat b);
void DrawOpenGL(OpenGlWindowState* pState);
LRESULT CALLBACK OpenGLWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND CreateOpenGlWindow(HINSTANCE hInstance, HWND parentWindow, Solver* solver) {
	OpenGlWindowState* state = new OpenGlWindowState();
	state->solver = solver;
	HWND window = CreateWindowEx(
		0,
		OPENGL_CLASS_NAME,
		L"OpenGL",
		WS_CHILD | WS_VISIBLE | WS_DISABLED,
		4, 4, 550, 550,
		parentWindow,
		NULL,
		hInstance,
		state
	);

	HDC hDC = GetDC(window);
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
		PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
		32,                   // Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0,
		0,
		0,
		0,
		0, 0, 0, 0,
		24,                   // Number of bits for the depthbuffer
		8,                    // Number of bits for the stencilbuffer
		0,                    // Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	int pf = ChoosePixelFormat(hDC, &pfd);
	if (!pf)
		MessageBox(window, L"Error", L"Error", MB_OK);
	if (!SetPixelFormat(hDC, pf, &pfd))
		MessageBox(window, L"SetPixelFormat", L"Error", MB_OK);

	//ReleaseDC(window, hDC);

	return window;
}

void DrawCircle(GLfloat x, GLfloat y, GLfloat radius, GLfloat r, GLfloat g, GLfloat b) {
	glColor3f(r, g, b);
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(x, y);
	for (double i = 0; i <= 36; ++i) {
		glVertex2f(x + radius * cos(i * 2 * M_PI / 36), y + radius * sin(i * 2 * M_PI / 36));
	}
	glEnd();
}

void DrawOpenGL(OpenGlWindowState* pState) {
	glClear(GL_COLOR_BUFFER_BIT);

	if (!pState->solver || pState->solver->nOrig <= 0) {
		glFlush();
		return;
	}

	Solver& solver = *pState->solver;

	std::vector<std::pair<double, double>> normPos(solver.nOrig);

	{
		double xMin, xMax, yMin, yMax;
		xMin = xMax = solver.positions[0].first;
		yMin = yMax = solver.positions[0].second;

		for (int i = 0; i < solver.nOrig; i++) {
			xMin = min(xMin, solver.positions[i].first);
			xMax = max(xMax, solver.positions[i].first);
			yMin = min(yMin, solver.positions[i].second);
			yMax = max(yMax, solver.positions[i].second);
		}

		double xDelta = xMax - xMin;
		double yDelta = yMax - yMin;

		double fieldLeft = -0.95, fieldBottom = -0.95;
		double fieldRight = 0.95, fieldTop = 0.95;
		double fieldWidth = fieldRight - fieldLeft, fieldHeight = fieldTop - fieldBottom;
		if (xDelta > yDelta)
			fieldHeight = fieldWidth * (yDelta / xDelta);
		else
			fieldWidth = fieldHeight * (xDelta / yDelta);
		fieldLeft = (2 - fieldWidth) / 2 - 1;
		fieldBottom = (2 - fieldHeight) / 2 - 1;
		
		for (int i = 0; i < solver.nOrig; i++) {
			double x = (solver.positions[i].first - xMin) / xDelta * fieldWidth + fieldLeft;
			double y = (solver.positions[i].second - yMin) / yDelta * fieldHeight + fieldBottom;

			normPos[i] = std::make_pair(x, y);
		}
	}

	for (int i = 0; i < solver.nOrig; i++) {
		for (int j = 0; j < solver.hOrig[i].size(); j++) {
			int to = solver.hOrig[i][j];
			glBegin(GL_LINES);
			glVertex2d(normPos[i].first, normPos[i].second);
			glVertex2d(normPos[to].first, normPos[to].second);
			glEnd();
		}
	}
	for (int i = 0; i < solver.nOrig; i++) {
		float r = .7;
		float g = .7;
		float b = .7;
		if (i == solver.source) {
			r = 0, g = 1, b = 0;
		}
		if (i == solver.sink) {
			r = 1, g = 0, b = 0;
		}
		DrawCircle(normPos[i].first, normPos[i].second, 0.02f, r, g, b);
	}

	glFlush();
}

LRESULT CALLBACK OpenGLWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	OpenGlWindowState* pState;
	if (uMsg == WM_CREATE)
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		pState = reinterpret_cast<OpenGlWindowState*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pState);
	}
	else {
		pState = reinterpret_cast<OpenGlWindowState*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	static PAINTSTRUCT ps;

	switch (uMsg)
	{
	case WM_PAINT:
	{
		HDC hDC = GetDC(hwnd);
		HGLRC hglrc;
		hglrc = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hglrc);

		RECT clientRect;
		GetClientRect(hwnd, &clientRect);
		glViewport(0, 0, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

		DrawOpenGL(pState);

		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(hglrc);

		SwapBuffers(hDC);
	}
	return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}