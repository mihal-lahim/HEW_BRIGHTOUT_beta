/*==============================================================================

   テクスチャの管理 [texture.h]
														 Author : Rintarou Sumi
														 Date   : 2025/06/6
--------------------------------------------------------------------------------

==============================================================================*/

#ifndef TEXTURE_H
#define TEXTURE_H
#include <d3d11.h>
#include <DirectXMath.h>
#include <string>



void Texture_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Texture_Finalize();

// テクスチャの読み込み
// 引数
// pFileName...画像ファイル名
// bMipMap...MipMapを作るときはtrue
// 
// 戻り値　テクスチャ管理番号　読み込めなかった場合は-1
// 
int Texture_Load(const wchar_t* pFileName,bool bMipMap = true);

void Texture_AllRelease();
void Texture_SetTexture(int texture_id);
DirectX::XMUINT2 Texture_GetSize(int texture_id);	// 複数版
const unsigned int& Texture_GetWidth(int texture_id);					// 一個一個版
const unsigned int& Texture_GetHeight(int texture_id);					// 一個一個版


#include "Resource.h"
#include <wrl/client.h>

class GraphicsDevice;

class Texture : public Resource
{
public:
	Texture() = default;
	virtual ~Texture() = default;
	// リソース初期化メソッド
	virtual void CreateBuffers(GraphicsDevice& device) override;

	// グラフィックスパイプラインにリソースをバインドするメソッド
	void BindResource(GraphicsDevice& device);

	void SetFilePath(const std::wstring& filePath) { m_filePath = filePath; }
private:
	std::wstring m_filePath;

	// テクスチャの幅と高さ
	unsigned int m_width = 0;
	unsigned int m_height = 0;

	// シェーダーリソースビュー
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView = nullptr;
};

#endif
