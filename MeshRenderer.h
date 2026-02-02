
#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "Component.h"
#include "Mesh.h"
#include "Shader.h"
#include <memory>

class Texture;
struct MODEL;


class MeshRenderer : public Component
{
public:
	MeshRenderer() = default;
	virtual ~MeshRenderer() = default;

	// MODELポインタ設定
	void SetModel(MODEL* model) 
	{ 
		m_model = model;
	}

	// メッシュポインタ設定
	void SetMesh(Mesh* mesh) 
	{ 
		m_mesh = mesh;
	}

	// テクスチャ設定
	void SetTexture(Texture* texture)
	{ 
		m_texture = texture;
	}

	// シェーダー設定
	void SetVertexShader(Shader* vertexShader)
	{
		m_vertexShader = vertexShader;
	}
	void SetPixelShader(Shader* pixelShader)
	{
		m_pixelShader = pixelShader;
	}

	// 描画処理
	void Render(GraphicsDevice& device);
private:
	// モデルへのポインタ（nullptrの場合モデル未設定）
	MODEL* m_model = nullptr;

	// メッシュへのポインタ（nullptrの場合、Cubeメッシュを使う）
	Mesh* m_mesh = nullptr;

	// テクスチャへのポインタ（nullptrの場合テクスチャ未設定）
	Texture* m_texture = nullptr;

	// 頂点シェーダーとピクセルシェーダー
	Shader* m_vertexShader = nullptr;
	Shader* m_pixelShader = nullptr;

	// 定数バッファ
	std::unique_ptr<ICB> m_vertexCB;
	std::unique_ptr<ICB> m_pixelCB;

};


#endif
