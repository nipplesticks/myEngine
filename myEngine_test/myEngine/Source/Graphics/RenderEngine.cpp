#include "RenderEngine.h"
#include "../Window/Window.h"
#define MAX_DESCRIPTOR_HEAP_SIZE 9999

namespace DEBUG
{
	HRESULT CreateError(const HRESULT& hr)
	{
		const _com_error err(hr);
		return CreateError(err.ErrorMessage());
	}

	HRESULT CreateError(const std::string& errorMsg)
	{
		return CreateError(std::wstring(errorMsg.begin(), errorMsg.end()));
	}

	HRESULT CreateError(const std::wstring& errorMsg)
	{
		return CreateError(LPCWSTR(errorMsg.c_str()));
	}

	HRESULT CreateError(const LPCWSTR& errorMsg)
	{
		MessageBoxW(nullptr, errorMsg,
			L"Error", MB_OK | MB_ICONERROR);
		return E_FAIL;
	}
}



RenderEngine::RenderEngine()
{

}

RenderEngine::~RenderEngine()
{

}

RenderEngine * RenderEngine::GetInstance()
{
	static RenderEngine renderer;
	return &renderer;
}

bool RenderEngine::Init()
{
	HRESULT hr = 0;

	IDXGIAdapter1 * adapter = nullptr;
	IDXGIFactory4 * factory = nullptr;

	if (FAILED(hr = _CheckD3D12Support(adapter, factory)))
	{
		SAFE_RELEASE(adapter);
		return DEBUG::CreateError(hr);
	}
#ifdef _DEBUG
	if (FAILED(hr = D3D12GetDebugInterface(IID_PPV_ARGS(&m_debugLayer))))
	{
		return DEBUG::CreateError(hr);
	}
	m_debugLayer->EnableDebugLayer();
#endif

	if (FAILED(hr = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device))))
	{
		SAFE_RELEASE(adapter);
		return DEBUG::CreateError(hr);
	}

	SAFE_RELEASE(adapter);

	if (FAILED(hr = _CreateCommandQueue()))
	{
		SAFE_RELEASE(factory);
		return !DEBUG::CreateError(hr);
	}

	if (FAILED(hr = _CreateSwapChain(factory)))
	{
		SAFE_RELEASE(factory);
		return !DEBUG::CreateError(hr);
	}
	SAFE_RELEASE(factory);


	if (FAILED(hr = _CreateRenderTargetDescriptorHeap()))
	{
		return !DEBUG::CreateError(hr);
	}

	if (FAILED(hr = _CreateCommandAllocators()))
	{
		return !DEBUG::CreateError(hr);
	}

	if (FAILED(hr = _CreateCommandList()))
	{
		return !DEBUG::CreateError(hr);
	}

	if (FAILED(hr = _CreateFenceAndFenceEvent()))
	{
		return !DEBUG::CreateError(hr);
	}

	if (FAILED(hr = _CreateResourceDescriptorHeap()))
	{
		return !DEBUG::CreateError(hr);
	}

	if (FAILED(hr = _CreateCPUDescriptorHeap()))
	{
		return !DEBUG::CreateError(hr);
	}

	if (FAILED(hr = _CreateCopyQueue()))
	{
		return !DEBUG::CreateError(hr);
	}

	return !hr;
}

bool RenderEngine::Flush()
{
	return false;
}

void RenderEngine::ClearGPU()
{
	if (m_fence->GetCompletedValue() < m_fenceValue)
	{
		m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent);
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	m_fenceValue++;
}

void RenderEngine::Release()
{
	SAFE_RELEASE(m_swapChain);
	SAFE_RELEASE(m_commandQueue);
	SAFE_RELEASE(m_rtvDescriptorHeap);
	SAFE_RELEASE(m_cpuDescriptorHeap);
	SAFE_RELEASE(m_copyQueue);
	SAFE_RELEASE(m_fence);

	for (UINT i = 0; i < FRAME_BUFFER_COUNT; i++)
	{
		SAFE_RELEASE(m_gpuDescriptorHeap[i]);
		SAFE_RELEASE(m_commandAllocator[i]);
		SAFE_RELEASE(m_renderTargets[i]);
		SAFE_RELEASE(m_commandList[i]);
		SAFE_RELEASE(m_copyCommandAllocator[i]);
		SAFE_RELEASE(m_copyCommandList[i]);
	}
	for (UINT i = 0; i < 5; i++)
	{
		m_passFences[i].Release();
	}
}

ID3D12Device * RenderEngine::GetDevice() const
{
	return m_device;
}

bool RenderEngine::Clear()
{
	memset(m_gpuOffset, 0, FRAME_BUFFER_COUNT * sizeof(SIZE_T));
}

HRESULT RenderEngine::_waitForPreviousFrame()
{
	HRESULT hr = 0;
	m_frameIndex = (m_frameIndex + 1) % FRAME_BUFFER_COUNT;
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	if (m_fence->GetCompletedValue() < m_fenceValue)
	{
		if (FAILED(hr = m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent)))
		{
			return hr;
		}
		WaitForSingleObject(m_fenceEvent, INFINITE);

	}
	m_fenceValue++;

	return hr;
}

HRESULT RenderEngine::_CheckD3D12Support(IDXGIAdapter1 *& adapter, IDXGIFactory4 *& dxgiFactory) const
{
	HRESULT hr = 0;
	if (adapter || dxgiFactory)
		return E_INVALIDARG;

	if (SUCCEEDED(hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory))))
	{
		UINT adapterIndex = 0;
		while (hr = dxgiFactory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter->GetDesc1(&desc);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				adapterIndex++;
				continue;
			}

			if (SUCCEEDED(hr = D3D12CreateDevice(adapter,
				D3D_FEATURE_LEVEL_12_0,
				_uuidof(ID3D12Device),
				nullptr)))
			{
				return hr;
			}
			SAFE_RELEASE(adapter);
		}

	}
	else
	{
		SAFE_RELEASE(dxgiFactory);
	}


	return hr;
}

HRESULT RenderEngine::_CreateCommandQueue()
{
	HRESULT hr = 0;

	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_DISABLE_GPU_TIMEOUT;
	if (FAILED(hr = this->m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue))))
	{
		SAFE_RELEASE(this->m_commandQueue);
	}
	SET_NAME(m_commandQueue, L"Default CommandQueue");
	return hr;
}

HRESULT RenderEngine::_CreateSwapChain(IDXGIFactory4 * dxgiFactory)
{
	HRESULT hr = 0;

	if (!dxgiFactory)
		return E_INVALIDARG;

	Window * wndPtr = Window::GetInstance();

	POINT windowSize = wndPtr->GetResolutionSize();

	DXGI_MODE_DESC backBufferDesc = {};
	backBufferDesc.Width = windowSize.x;
	backBufferDesc.Height = windowSize.y;
	backBufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1;

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = FRAME_BUFFER_COUNT;
	swapChainDesc.BufferDesc = backBufferDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.OutputWindow = wndPtr->GetHwnd();
	swapChainDesc.SampleDesc = sampleDesc;
	swapChainDesc.Windowed = !wndPtr->IsFullscreen();

	IDXGISwapChain * tmpSwapChain = nullptr;
	if (SUCCEEDED(hr = dxgiFactory->CreateSwapChain(m_commandQueue,
		&swapChainDesc,
		&tmpSwapChain)))
	{
		if (FAILED(hr = tmpSwapChain->QueryInterface(IID_PPV_ARGS(&m_swapChain))))
		{
			SAFE_RELEASE(m_swapChain);
			return E_FAIL;
		}
	}
	SAFE_RELEASE(tmpSwapChain);
	return hr;
}

HRESULT RenderEngine::_CreateRenderTargetDescriptorHeap()
{
	HRESULT hr = 0;

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = FRAME_BUFFER_COUNT;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	if (SUCCEEDED(hr = m_device->CreateDescriptorHeap(
		&rtvHeapDesc,
		IID_PPV_ARGS(&m_rtvDescriptorHeap))))
	{
		m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{ m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr };

		for (UINT i = 0; i < FRAME_BUFFER_COUNT; i++)
		{
			if (FAILED(hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i]))))
			{
				break;
			}
			m_device->CreateRenderTargetView(m_renderTargets[i], nullptr, rtvHandle);
			rtvHandle.ptr += m_rtvDescriptorSize;
			SET_NAME(m_renderTargets[i], std::wstring(std::wstring(L"Default RenderTarget") + std::to_wstring(i)).c_str());
		}
		SET_NAME(m_rtvDescriptorHeap, L"RenderTargetViewDescriptorHeap");
	}
	return hr;
}

HRESULT RenderEngine::_CreateCommandAllocators()
{
	HRESULT hr = 0;

	for (UINT i = 0; i < FRAME_BUFFER_COUNT; i++)
	{
		if (FAILED(hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator[i]))))
		{
			break;
		}
		SET_NAME(m_commandAllocator[i], std::wstring(std::wstring(L"Default CommandAllocator") + std::to_wstring(i)).c_str());
	}

	return hr;
}

HRESULT RenderEngine::_CreateCommandList()
{
	HRESULT hr = 0;
	for (UINT i = 0; i < FRAME_BUFFER_COUNT; i++)
	{
		if (SUCCEEDED(hr = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator[0], nullptr, IID_PPV_ARGS(&m_commandList[i]))))
		{
			SET_NAME(m_commandList[i], L"Default CommandList");
			m_commandList[i]->Close();
		}
		else
			return hr;
	}
	return hr;
}

HRESULT RenderEngine::_CreateFenceAndFenceEvent()
{
	HRESULT hr = 0;

	if (FAILED(hr = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence))))
	{
		return hr;
	}
	m_fenceValue = 0;


	m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (nullptr == m_fenceEvent)
		return E_FAIL;

	std::wstring arr[] = {
		L"Fence_UPDATE_PASS",
		L"Fence_PARTICLE_PASS",
		L"Fence_DEPTH_PRE_PASS",
		L"Fence_SHADOW_PASS",
		L"Fence_GEOMETRY_PASS",
	};

	for (UINT i = 0; i < 5; i++)
	{
		if (FAILED(hr = m_passFences[i].CreateFence(arr[i])))
			return hr;
	}

	return hr;
}

HRESULT RenderEngine::_CreateResourceDescriptorHeap()
{
	HRESULT hr = 0;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.NumDescriptors = MAX_DESCRIPTOR_HEAP_SIZE;

	for (UINT i = 0; i < FRAME_BUFFER_COUNT; i++)
	{
		if (FAILED(hr = m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_gpuDescriptorHeap[i]))))
			return hr;
		SET_NAME(m_gpuDescriptorHeap[i], std::wstring(std::wstring(L"GPU DescriptorHeap ") + std::to_wstring(i)).c_str());
	}

	return hr;
}

HRESULT RenderEngine::_CreateCPUDescriptorHeap()
{
	HRESULT hr = 0;

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc.NumDescriptors = MAX_DESCRIPTOR_HEAP_SIZE;

	if (FAILED(hr = m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_cpuDescriptorHeap))))
		return hr;

	SET_NAME(m_cpuDescriptorHeap, L"CPU DescriptorHeap");

	m_resourceDescriptorHeapSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return hr;
}

HRESULT RenderEngine::_CreateCopyQueue()
{
	HRESULT hr = 0;

	D3D12_COMMAND_QUEUE_DESC desc{};
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

	if (FAILED(hr = m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_copyQueue))))
		return hr;

	for (UINT i = 0; i < FRAME_BUFFER_COUNT; i++)
	{
		if (FAILED(hr = m_device->CreateCommandAllocator(
			D3D12_COMMAND_LIST_TYPE_COPY,
			IID_PPV_ARGS(&m_copyCommandAllocator[i]))))
			return hr;

		SET_NAME(m_copyCommandAllocator[i], L"Copy");

		if (FAILED(hr = m_device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_COPY,
			m_copyCommandAllocator[i],
			nullptr,
			IID_PPV_ARGS(&m_copyCommandList[i]))))
			return hr;
		SET_NAME(m_copyCommandList[i], L"Copy");

		m_copyCommandList[i]->Close();
	}

	return hr;
}
