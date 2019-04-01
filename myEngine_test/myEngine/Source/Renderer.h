#pragma once
#include <Windows.h>
#include <string>

class Window;
class RenderEngine;

class Renderer
{
public:
	Renderer();
	~Renderer();

	bool Init(HINSTANCE hInstance, UINT width, UINT height, BOOL fullscreen = false, 
		const std::string & windowTitle = "myEngineTitle", const std::string & windowName = "myEngine");

	void Clear();
	void Flush();

	bool IsRunning() const;
private:
	Window * m_windowPtr = nullptr;
	RenderEngine * m_renderEnginePtr = nullptr;

};