#include "Renderer.h"
#include "Window/Window.h"
#include "Graphics/RenderEngine.h"


Renderer::Renderer()
{
	m_windowPtr = Window::GetInstance();
	m_renderEnginePtr = RenderEngine::GetInstance();
}

Renderer::~Renderer()
{
	
}

bool Renderer::Init(HINSTANCE hInstance, UINT width, UINT height, BOOL fullscreen, const std::string & windowTitle, const std::string & windowName)
{
	if (!m_windowPtr->Create(hInstance, 10, width, height, fullscreen, windowName, windowTitle))
	{
		return false;
	}

	if (FAILED(m_renderEnginePtr->Init()))
	{
		return false;
	}

	return true;
}

void Renderer::Clear()
{
	m_renderEnginePtr->Clear();
}

void Renderer::Flush()
{
	m_renderEnginePtr->Flush();
}

void Renderer::Terminate()
{
	m_windowPtr->Close();
}

void Renderer::Release()
{
	m_windowPtr->Close();
	m_renderEnginePtr->ClearGPU();
	m_renderEnginePtr->Release();
}

bool Renderer::IsRunning() const
{
	return m_windowPtr->IsOpen();
}
