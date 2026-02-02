#ifndef GRAPHICS_DEVICE_H
#define GRAPHICS_DEVICE_H

#include <d3d11.h>
#include <Windows.h>
#include <memory>
#include <wrl/client.h>

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=nullptr; } }

class GraphicsDevice
{
public:
	GraphicsDevice() = default;
	~GraphicsDevice() = default;

	// 初期化
	bool Initialize(HWND hWnd);
	void Finalize();

	void Clear();
	void Present();

	unsigned int GetBackBufferWidth() const;
	unsigned int GetBackBufferHeight() const;


	void SetViewport(int n);

	enum AlphaBlendMode
	{
		BLEND_TRANSPARENT,
		BLEND_OPAQUE,
		BLEND_ADD,
		BLEND_MAX
	};

	void SetAlphaBlend(AlphaBlendMode blend);
	void SetDepthTest(bool bEnable);

	// デバイスとコンテキストの取得
	ID3D11Device* const GetDevice() const { return m_device.Get(); }
	ID3D11DeviceContext* const GetDeviceContext() const { return m_deviceContext.Get(); }
private:

	// バックバッファの作成
	bool CreateBackBuffers();

	// Direct3Dデバイスとデバイスコンテキスト
	Microsoft::WRL::ComPtr<ID3D11Device> m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;

	// スワップチェイン
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;

	// レンダーターゲットビュー
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;

	// デプスステンシルビュー
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilBuffer;

	// ブレンドステート
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendStateMultiply;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendStateOpaque;
	Microsoft::WRL::ComPtr<ID3D11BlendState> m_blendStateAdd;

	// デプスステンシルステート
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilStateDepthDisable;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthStencilStateDepthEnable;

	// ビューポート
	D3D11_VIEWPORT m_viewport[2]{};

	// バックバッファの情報
	D3D11_TEXTURE2D_DESC m_backBufferDesc{};
};

GraphicsDevice& GetGraphicsDevice();

#endif
