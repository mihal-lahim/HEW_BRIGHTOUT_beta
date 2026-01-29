#include "MeshRenderer.h"
#include "model.h"
#include "GameObject.h"
#include "cube.h"

void MeshRenderer::Render()
{
	// トランスフォーム取得
	const Transform& tf = gameObject()->transform;

	// ワールド行列計算
	DirectX::XMMATRIX mtxWorld =
		DirectX::XMMatrixScaling(tf.Scale.x, tf.Scale.y, tf.Scale.z) *
		tf.Rotation.ToXMMATRIX() *
		DirectX::XMMatrixTranslation(tf.Position.x, tf.Position.y, tf.Position.z);

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
