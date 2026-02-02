
#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "Component.h"
#include "Mesh.h"
#include "Shader.h"
#include "Material.h"
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
	void SetVertexShader(VertexShader3D* vertexShader)
	{
		m_vertexShader = vertexShader;
	}

	VertexShader3D* GetVertexShader() const
	{
		return m_vertexShader;
	}

	void SetMaterialInstance(MaterialInstance3D* materialInstance)
	{
		m_materialInstance = materialInstance;
	}

	MaterialInstance3D* GetMaterialInstance() const
	{
		return m_materialInstance;
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

	// 頂点シェーダー
	VertexShader3D* m_vertexShader = nullptr;

	// マテリアル
	MaterialInstance3D* m_materialInstance = nullptr;
};


#endif
