#include "Fence.h"
#include "../RenderEngine.h"

Fence::Fence()
{
}

Fence::~Fence()
{
}

HRESULT Fence::CreateFence(const std::wstring & name)
{
	HRESULT hr = 0;


	RenderEngine * renderer = RenderEngine::GetInstance();

	if (SUCCEEDED(renderer->GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence))))
	{
		m_fence->SetName(name.c_str());
		m_fenceValue = 0;
	}
	return hr;
}

HRESULT Fence::Signal(ID3D12CommandQueue * commandQueue)
{
	m_fenceValue++;
	HRESULT hr = commandQueue->Signal(m_fence, m_fenceValue);
	return hr;
}

HRESULT Fence::Wait(ID3D12CommandQueue * commandQueue)
{
	HRESULT hr = 0;

	if (m_fence->GetCompletedValue() < m_fenceValue)
	{
		hr = commandQueue->Wait(m_fence, m_fenceValue);
	}

	return hr;
}

void Fence::Release()
{
	SAFE_RELEASE(m_fence);
}
