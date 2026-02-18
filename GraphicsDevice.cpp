#include "GraphicsDevice.h"

#pragma comment(lib, "d3d11.lib")

#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "DirectXTex_Debug.lib")
#else
#pragma comment(lib, "DirectXTex_Release.lib")
#endif


bool GraphicsDevice::Initialize(HWND hWnd)
{
	//デバイス、スワップチェイン、コンテキスト生成
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainDesc.OutputWindow = hWnd;


	UINT deviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	// デバイスとスワップチェインの作成
	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		deviceFlags,
		levels,
		ARRAYSIZE(levels),
		D3D11_SDK_VERSION,
		&swapChainDesc,
		m_swapChain.GetAddressOf(),
		&m_device,
		&featureLevel,
		&m_deviceContext);


	if (FAILED(hr)) {
		MessageBox(hWnd, "Direct3Dの初期化に失敗しました", "エラー", MB_OK);
		return false;
	}

	if (!CreateBackBuffers()) {
		MessageBox(hWnd, "バックバッファの設定に失敗しました", "エラー", MB_OK);
		return false;
	}

	return true;
}

void GraphicsDevice::Finalize()
{
	m_swapChain.Reset();
	m_deviceContext.Reset();
	m_device.Reset();

	m_samplerState.Reset();
	m_blendStateMultiply.Reset();
	m_blendStateOpaque.Reset();
	m_blendStateAdd.Reset();
	m_depthStencilStateDepthDisable.Reset();
	m_depthStencilStateDepthEnable.Reset();
}

void GraphicsDevice::Clear()
{
	m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), m_clearColor);
	m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	ID3D11RenderTargetView* renderTargetView = m_renderTargetView.Get();
	m_deviceContext->OMSetRenderTargets(1, &renderTargetView, m_depthStencilView.Get());
}

void GraphicsDevice::SetClearColor(float r, float g, float b, float a)
{
	m_clearColor[0] = r;
	m_clearColor[1] = g;
	m_clearColor[2] = b;
	m_clearColor[3] = a;
}

void GraphicsDevice::Present()
{
	m_swapChain->Present(1, 0);
}

unsigned int GraphicsDevice::GetBackBufferWidth() const
{
	return m_backBufferDesc.Width;
}

unsigned int GraphicsDevice::GetBackBufferHeight() const
{
	return m_backBufferDesc.Height;
}

void GraphicsDevice::SetViewport(int n)
{
	m_deviceContext->RSSetViewports(1, &m_viewport[n]);
}

void GraphicsDevice::SetAlphaBlend(AlphaBlendMode blend)
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ID3D11BlendState* blendState = nullptr;

	switch (blend)
	{
	case BLEND_TRANSPARENT:
		blendState = m_blendStateMultiply.Get();
		break;
	case BLEND_OPAQUE:
		blendState = m_blendStateOpaque.Get();
		break;
	case BLEND_ADD:
		blendState = m_blendStateAdd.Get();
		break;
	}

	m_deviceContext->OMSetBlendState(blendState, blendFactor, 0xffffffff);
}

void GraphicsDevice::SetDepthTest(bool bEnable)
{
	if (bEnable) {
		m_deviceContext->OMSetDepthStencilState(m_depthStencilStateDepthEnable.Get(), NULL);
	} else {
		m_deviceContext->OMSetDepthStencilState(m_depthStencilStateDepthDisable.Get(), NULL);
	}
}

bool GraphicsDevice::CreateBackBuffers()
{
	HRESULT hr{};
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBufferPointer;
	// バックバッファの取得
	hr = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBufferPointer));

	if (FAILED(hr)) {
		return false;
	}

	// バックバッファのレンダーターゲットビューの作成
	hr = m_device->CreateRenderTargetView(backBufferPointer.Get(), nullptr, m_renderTargetView.GetAddressOf());
	if (FAILED(hr)) {
		return false;
	}
	// バックバッファの情報（幅）取得
	backBufferPointer->GetDesc(&m_backBufferDesc);

	// デプスステンシルビューの作成
	D3D11_TEXTURE2D_DESC depthStencilDesc{};
	depthStencilDesc.Width = m_backBufferDesc.Width;
	depthStencilDesc.Height = m_backBufferDesc.Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	// デプスステンシルバッファの作成
	hr = m_device->CreateTexture2D(&depthStencilDesc, nullptr, m_depthStencilBuffer.GetAddressOf());

	if (FAILED(hr)) {
		return false;
	}

	// デプスステンシルビューの作成
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = depthStencilDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	depthStencilViewDesc.Flags = 0;

	// デプスステンシルビューの作成
	hr = m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &depthStencilViewDesc, m_depthStencilView.GetAddressOf());

	if (FAILED(hr)) {
		return false;
	}

	// ビューポートの設定
	m_viewport[0].TopLeftX = 0.0f;
	m_viewport[0].TopLeftY = 0.0f;
	m_viewport[0].Width = static_cast<FLOAT>(m_backBufferDesc.Width);
	m_viewport[0].Height = static_cast<FLOAT>(m_backBufferDesc.Height);
	m_viewport[0].MinDepth = 0.0f;
	m_viewport[0].MaxDepth = 1.0f;

	m_viewport[1].TopLeftX = static_cast<FLOAT>(m_backBufferDesc.Width) * 0.5f;
	m_viewport[1].TopLeftY = 0.0f;
	m_viewport[1].Width = static_cast<FLOAT>(m_backBufferDesc.Width) * 0.5f;
	m_viewport[1].Height = static_cast<FLOAT>(m_backBufferDesc.Height);
	m_viewport[1].MinDepth = 0.0f;
	m_viewport[1].MaxDepth = 1.0f;

	// 最初はフルサイズビューポートをセット
	m_deviceContext->RSSetViewports(1, &m_viewport[0]);

	D3D11_BLEND_DESC bd = {};
	bd.AlphaToCoverageEnable = FALSE;
	bd.IndependentBlendEnable = FALSE;
	bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	// ブレンドステートの作成
	m_device->CreateBlendState(&bd, m_blendStateMultiply.GetAddressOf());

	// 加算用ブレンドステートの作成
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	m_device->CreateBlendState(&bd, m_blendStateAdd.GetAddressOf());

	// 不透明用ブレンドステートの作成
	D3D11_BLEND_DESC bdOpaque = bd;
	bdOpaque.RenderTarget[0].BlendEnable = FALSE;
	m_device->CreateBlendState(&bdOpaque, m_blendStateOpaque.GetAddressOf());

	// 初期状態は透過
	SetAlphaBlend(BLEND_TRANSPARENT);

	// デプスステンシルステートの作成
	D3D11_DEPTH_STENCIL_DESC dsd = {};
		dsd.DepthFunc = D3D11_COMPARISON_LESS;
		dsd.StencilEnable = FALSE;
		dsd.DepthEnable = FALSE;
		dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	// 深度テスト無効ステートの作成
	m_device->CreateDepthStencilState(&dsd, m_depthStencilStateDepthDisable.GetAddressOf());

	// 深度テスト有効ステートの作成
	dsd.DepthEnable = TRUE;
	dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	m_device->CreateDepthStencilState(&dsd, m_depthStencilStateDepthEnable.GetAddressOf());

	// 初期状態は深度テスト無効
	m_deviceContext->OMSetDepthStencilState(m_depthStencilStateDepthDisable.Get(), NULL);
	SetDepthTest(false);

	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	hr = m_device->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf());
	if (FAILED(hr)) {
		return false;
	}

	return true;
}
