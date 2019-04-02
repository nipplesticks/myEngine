#pragma once
#include <string>
#include <stdint.h>
#include <Windows.h>
#include <comdef.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <dxgi.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#pragma comment (lib, "d3d12.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment (lib, "DXGI.lib")

#define SAFE_RELEASE(p) {if ((p)) {(p)->Release(); (p) = nullptr;}}
#define SAFE_DELETE(p) {if ((p)) {delete (p); (p) = nullptr;}}
#define SET_NAME(p, str) {if ((p)){ (p)->SetName((str));}}
#define AlignAs256(n){ n + 255 & ~255 }

const UINT FRAME_BUFFER_COUNT = 4u;

namespace DEBUG
{
	extern HRESULT CreateError(const std::string& errorMsg);
	extern HRESULT CreateError(const std::wstring& errorMsg);
	extern HRESULT CreateError(const LPCWSTR& errorMsg);
	extern HRESULT CreateError(const HRESULT& hr);
}

namespace myStructs
{
	struct Vertex
	{
		DirectX::XMFLOAT4 Position;
		DirectX::XMFLOAT4 Normal;
		DirectX::XMFLOAT4 Tangent;
		DirectX::XMFLOAT4 biTangent;
		DirectX::XMFLOAT4 UV;
	};
}