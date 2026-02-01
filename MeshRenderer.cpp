#include "MeshRenderer.h"
#include "model.h"
#include "GameObject.h"
#include "cube.h"

void MeshRenderer::Render()
{
	// トランスフォーム取得
	Transform& tf = gameObject().transform();

	// ワールド行列計算
	DirectX::XMMATRIX mtxWorld =
		DirectX::XMMatrixScaling(tf.scale().x, tf.scale().y, tf.scale().z) *
		tf.rotation().ToXMMATRIX() *
		DirectX::XMMatrixTranslation(tf.position().x, tf.position().y, tf.position().z);

	// モデルが設定されている場合
	if (m_Model)
	{
		// モデル描画
		ModelDraw(m_Model, mtxWorld);
	}
	else
	{
		Cube_Draw(m_TextureIndex, mtxWorld);
	}
}
