#include <windows.h>
#include <GL/GL.h>
#include "globals.h"
#include <string>

#pragma comment (lib, "opengl32.lib")

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//hacky and horrible, but its for a start
HWND g_MainWindow = NULL;
HDC g_WindowHandleToDeviceContext = NULL;
HGLRC g_OpenGLRenderingContext = NULL;
int  g_PixelFormat = NULL;
float hacketyHackRotation = 0.f;
int hacketyHackDirection = 0;
float hacketyHackPositionY = 0;
float hacketyHackMonkeyWrenchThrownToAngle = 1.f;

void ShowLastWindowsError();
void InitializeWindowsOpenGL();
void InitializeOpenGL();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{

	MSG msg          = {0};
	WNDCLASS wc      = {0}; 
	wc.style		 = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = WndProc;
	wc.hInstance     = hInstance;
	wc.cbClsExtra	= 0;
	wc.cbWndExtra	= 0;
	wc.hIcon		 = LoadIcon( NULL, IDI_APPLICATION );
	wc.hCursor		 = LoadCursor( NULL, IDC_ARROW );
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND);
	wc.lpszClassName = PROGRAM_NAME_AND_VERSION;
	wc.lpszMenuName = NULL;
	if( !RegisterClass(&wc) )
		return 1;

	g_MainWindow = CreateWindow(wc.lpszClassName,
								PROGRAM_NAME_AND_VERSION,
								WS_OVERLAPPEDWINDOW|WS_VISIBLE,
								0,0,640,480,0,0,hInstance,NULL);

	if ( !g_MainWindow )
		return 2;

	InitializeWindowsOpenGL();
	InitializeOpenGL();

	ShowWindow ( g_MainWindow, WS_VISIBLE );
	UpdateWindow ( g_MainWindow );

	while( GetMessage( &msg, NULL, 0, 0 ) > 0 )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return 0;
}

void InitializeWindowsOpenGL()
{

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(pfd));
	pfd.nSize		= sizeof( pfd );
	pfd.nVersion	= 1;
	pfd.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType	= PFD_TYPE_RGBA;
	pfd.cColorBits	= 32;
	pfd.cDepthBits	= 16;
	pfd.iLayerType	= PFD_MAIN_PLANE;
	pfd.cAlphaBits	= 8;
	pfd.cRedBits	= 8;
	pfd.cGreenBits	= 8;
	pfd.cBlueBits	= 8;

	g_WindowHandleToDeviceContext = GetDC(g_MainWindow);
	if(!g_WindowHandleToDeviceContext)
		ShowLastWindowsError();

	g_PixelFormat = ChoosePixelFormat(g_WindowHandleToDeviceContext, &pfd); 
	SetPixelFormat(g_WindowHandleToDeviceContext,g_PixelFormat, &pfd);

	g_OpenGLRenderingContext = wglCreateContext(g_WindowHandleToDeviceContext);
	if(!g_OpenGLRenderingContext)
		ShowLastWindowsError();

	if ( !wglMakeCurrent (g_WindowHandleToDeviceContext, g_OpenGLRenderingContext) )
		ShowLastWindowsError();

}

void InitializeOpenGL()
{
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	glLoadIdentity();
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_KEYUP:
		switch(wParam)
		{
		case VK_UP:
		case VK_DOWN:
			hacketyHackPositionY = 0;
			break;
		}
		break;
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_UP:
			hacketyHackPositionY = 0.01f;
			break;

		case VK_DOWN:
			hacketyHackPositionY = -0.01f;
			break;

		case VK_LEFT:
			hacketyHackDirection = 1;
			break;

		case VK_RIGHT:
			hacketyHackDirection = 0;
			break;

		case VK_ESCAPE:
			wglDeleteContext(g_OpenGLRenderingContext);
			PostQuitMessage(0);
			break;
		}
		break;
	case WM_PAINT:
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
		//glLoadIdentity();//<- this would reset all transforms etc. which would be boring :<
		glClearColor(0.6f,0.6f,0.f,1.f);

		hacketyHackRotation += 0.1f;
		if ( hacketyHackDirection == 1 )
			hacketyHackMonkeyWrenchThrownToAngle = 1.f;
		else
			hacketyHackMonkeyWrenchThrownToAngle = -1.f;

		glTranslatef(0.f,hacketyHackPositionY,0.f);
		glRotatef(hacketyHackMonkeyWrenchThrownToAngle,0.f,0.f,hacketyHackRotation);
		glBegin(GL_TRIANGLES);
			glColor3f(0.1, 0.2, 0.3);
			glVertex3f(0, 0, 0);
			glVertex3f(0.1, 0, 0);
			glVertex3f(0, 0.3, 0);
		glEnd();
		SwapBuffers(g_WindowHandleToDeviceContext);
		break;
	case WM_CREATE:
		//Avoid putting stuff here, this place is risky and sucks, especially for opengl initializations etc. because everything might not be ready yet.
		break;
	case WM_CLOSE:
		wglDeleteContext(g_OpenGLRenderingContext);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;

}

void ShowLastWindowsError()
{
	void* lpBuffer;

	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		::GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
		(LPTSTR) &lpBuffer,
		0,
		NULL );

	MessageBox( NULL, (LPCTSTR)lpBuffer, "LastError", MB_OK );

	LocalFree( lpBuffer );
}
