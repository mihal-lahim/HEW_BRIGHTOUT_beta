
#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "Component.h"
#include "Mesh.h"

class Texture;
struct MODEL;

class MeshRenderer : public Component
{
public:
	MeshRenderer() = default;
	virtual ~MeshRenderer() = default;

	// MODELポインタ設定
	void SetModel(MODEL* model) { m_model = model; }

	// メッシュポインタ設定
	void SetMesh(Mesh* mesh) { m_mesh = mesh; }

	// テクスチャ設定
	void SetTexture(Texture* texture) { m_texture = texture; }

	// 描画処理
	void Render(GraphicsDevice& device);
private:
	// モデルへのポインタ（nullptrの場合モデル未設定）
	MODEL* m_model = nullptr;

	// メッシュへのポインタ（nullptrの場合、Cubeメッシュを使う）
	Mesh* m_mesh = nullptr;

	Texture* m_texture = nullptr;
};


#endif
