/*==============================================================================

   テクスチャの管理 [texture.h]
														 Author : Rintarou Sumi
														 Date   : 2025/06/6
--------------------------------------------------------------------------------

==============================================================================*/

#ifndef TEXTURE_H
#define TEXTURE_H
#include <d3d11.h>
#include <string>
#include "Resource.h"
#include <wrl/client.h>

class GraphicsDevice;

class Texture : public Resource
{
public:
	Texture() = default;
	virtual ~Texture() = default;
	// リソース初期化メソッド
	virtual bool CreateBuffer(GraphicsDevice& device, const std::wstring& filePath);

	// グラフィックスパイプラインにリソースをバインドするメソッド
	void Bind(GraphicsDevice& device);

	// テクスチャの幅と高さを取得するメソッド
	unsigned int GetWidth() const 
	{ 
		return m_width;
	}
	unsigned int GetHeight() const 
	{ 
		return m_height;
	}
private:
	// テクスチャファイルのパス
	std::wstring m_filePath = {};

	// テクスチャの幅と高さ
	unsigned int m_width = 0;
	unsigned int m_height = 0;

	// シェーダーリソースビュー
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView = nullptr;
};

#endif
