
#ifndef MESHRENDERER_H
#define MESHRENDERER_H

#include "Component.h"
#include "Mesh.h"

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

	// テクスチャインデックス設定
	void SetTextureIndex(int index) { m_textureIndex = index; }

	// 描画処理
	void Render(GraphicsDevice* device);
private:
	// モデルへのポインタ（nullptrの場合モデル未設定）
	MODEL* m_model = nullptr;

	// メッシュへのポインタ（nullptrの場合、Cubeメッシュを使う）
	Mesh* m_mesh = nullptr;

	// テクスチャインデックス（MODELがnullptrの場合、Cubeに貼るテクスチャのインデックス）
	int m_textureIndex = -1;
};


#endif
