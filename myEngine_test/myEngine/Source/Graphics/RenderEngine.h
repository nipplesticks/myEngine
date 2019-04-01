#pragma once
#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <dxgi.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "DXGI.lib")

class RenderEngine
{
private:
	RenderEngine();
	~RenderEngine();

public:
	static RenderEngine * GetInstance();

	HRESULT Init();
	HRESULT Clear();
	HRESULT Flush();


	void ClearGPU();
	void Release();

private:
};