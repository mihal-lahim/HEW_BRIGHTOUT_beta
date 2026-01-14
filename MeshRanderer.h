
#ifndef MESHRANDERER_H
#define MESHRANDERER_H

#include "Component.h"

class MODEL;

class MeshRenderer : public Component
{
private:
	// モデルへのポインタ（nullptrの場合モデル未設定）
	MODEL* m_Model = nullptr;

	// テクスチャインデックス（MODELがnullptrの場合、Cubeに貼るテクスチャのインデックス）
	int m_TextureIndex = -1;
public:

	// MODELポインタ設定
	void SetModel(MODEL* model) { m_Model = model; }

	// テクスチャインデックス設定
	void SetTextureIndex(int index) { m_TextureIndex = index; }

	// 描画処理
	void Render();
};


#endif
