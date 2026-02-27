#ifndef GRAPHICS_DEVICE_H
#define GRAPHICS_DEVICE_H

#include <d3d11.h>
#include <Windows.h>
#include <memory>
#include <wrl/client.h>



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

	// クリア色の設定メソッド
	void SetClearColor(float r, float g, float b, float a = 1.0f);

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

	enum CullMode
	{
		CULL_BACK,
		CULL_FRONT,
		CULL_NONE
	};

	void SetCullMode(CullMode mode);

	// デバイスとコンテキストの取得
	ID3D11Device* const GetDevice() const { return m_device.Get(); }
	ID3D11DeviceContext* const GetDeviceContext() const { return m_deviceContext.Get(); }
	ID3D11SamplerState* const GetSamplerState() const { return m_samplerState.Get(); }
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

	// サンプラーステート
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerState;

	// ビューポート
	D3D11_VIEWPORT m_viewport[2]{};

	// バックバッファの情報
	D3D11_TEXTURE2D_DESC m_backBufferDesc{};

	// クリア色
	float m_clearColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

	// ラスタライザーステート
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rsCullBack;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rsCullFront;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_rsCullNone;

};


#endif
