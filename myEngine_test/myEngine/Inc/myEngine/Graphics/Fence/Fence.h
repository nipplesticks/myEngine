#pragma once
#include <string>
#include "../GraphicsExtern.h"


class Fence
{
public:
	Fence();
	~Fence();
	HRESULT CreateFence(const std::wstring & name);

	HRESULT Signal(ID3D12CommandQueue * commandQueue);
	HRESULT Wait(ID3D12CommandQueue * commandQueue);
	
	void Release();
private:
	ID3D12Fence *			m_fence = nullptr;
	UINT64 					m_fenceValue = 0;
};

