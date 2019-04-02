#pragma once
#include "GraphicsExtern.h"
#include "Fence/Fence.h"

#define UPDATE_PASS 0
#define PARTICLE_PASS 0
#define DEPTH_PRE_PASS 0
#define SHADOW_PASS 0
#define GEOMETRY_PASS 0

class RenderEngine
{
private:
	RenderEngine();
	~RenderEngine();

public:
	static RenderEngine * GetInstance();

	bool Init();

	bool Clear();

	bool Flush();

	void ClearGPU();

	void Release();

	ID3D12Device * GetDevice() const;


private:
	ID3D12Device *				m_device = nullptr;
	IDXGISwapChain3 *			m_swapChain = nullptr;
	ID3D12CommandQueue *		m_commandQueue = nullptr;

	ID3D12DescriptorHeap *		m_rtvDescriptorHeap = nullptr;
	ID3D12DescriptorHeap *		m_gpuDescriptorHeap[FRAME_BUFFER_COUNT] = { nullptr };

	ID3D12CommandQueue	*		m_copyQueue = nullptr;
	ID3D12CommandAllocator *	m_copyCommandAllocator[FRAME_BUFFER_COUNT] = { nullptr };
	ID3D12GraphicsCommandList * m_copyCommandList[FRAME_BUFFER_COUNT] = { nullptr };


	ID3D12DescriptorHeap *		m_cpuDescriptorHeap = nullptr;
	SIZE_T						m_currentResourceIndex = 0;
	SIZE_T						m_resourceDescriptorHeapSize = 0;

	ID3D12GraphicsCommandList * m_commandList[FRAME_BUFFER_COUNT]{ nullptr };
	ID3D12CommandAllocator *	m_commandAllocator[FRAME_BUFFER_COUNT]{ nullptr };
	ID3D12Resource *			m_renderTargets[FRAME_BUFFER_COUNT]{ nullptr };

	ID3D12Fence *				m_fence = nullptr;
	UINT64 						m_fenceValue = 0;

	HANDLE m_fenceEvent = nullptr;
	SIZE_T m_gpuOffset[FRAME_BUFFER_COUNT] = { 0 };

	Fence m_passFences[3];

	UINT m_frameIndex = 0;
	UINT m_presentIndex = 0;
	UINT m_rtvDescriptorSize = 0;

private:
	HRESULT _waitForPreviousFrame();
	HRESULT _CheckD3D12Support(IDXGIAdapter1 *& adapter, IDXGIFactory4 *& dxgiFactory) const;
	HRESULT _CreateCommandQueue();
	HRESULT _CreateSwapChain(IDXGIFactory4 * dxgiFactory);
	HRESULT _CreateRenderTargetDescriptorHeap();
	HRESULT _CreateCommandAllocators();
	HRESULT _CreateCommandList();
	HRESULT _CreateFenceAndFenceEvent();
	HRESULT _CreateResourceDescriptorHeap();
	HRESULT _CreateCPUDescriptorHeap();
	HRESULT _CreateCopyQueue();


private: // DEBUG
	ID3D12Debug * m_debugLayer = nullptr;
};