#include <myEngine/Renderer.h>
#include <myEngine/Window/Window.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
	Renderer renderer;
	renderer.Init(hInstance, 1280, 720);


	while (renderer.IsRunning())
	{
		renderer.Clear();

		if (Window::GetInstance()->IsKeyPressed(Input::ESCAPE))
		{
			renderer.Terminate();
		}

		renderer.Flush();
	}

	renderer.Release();

	return 0;
}