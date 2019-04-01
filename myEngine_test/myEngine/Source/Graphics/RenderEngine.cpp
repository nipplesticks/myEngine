#include "RenderEngine.h"

RenderEngine::~RenderEngine()
{
}

RenderEngine * RenderEngine::GetInstance()
{
	return nullptr;
}

HRESULT RenderEngine::Init()
{
	return E_NOTIMPL;
}

HRESULT RenderEngine::Flush()
{
	return E_NOTIMPL;
}

void RenderEngine::ClearGPU()
{
}

void RenderEngine::Release()
{
}

HRESULT RenderEngine::Clear()
{
	return E_NOTIMPL;
}
