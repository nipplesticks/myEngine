#include <windows.h>

#include <myEngine/Window/Window.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Window * wnd = Window::GetInstance();

	wnd->Create(hInstance, nCmdShow, 1280, 720, NULL);

	int counter = 0;

	while (wnd->IsOpen())
	{
		if (wnd->IsKeyPressed(Input::ESCAPE))
			wnd->Close();
	}


	return 0;
}