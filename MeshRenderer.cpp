#include "MeshRenderer.h"
#include "model.h"
#include "GameObject.h"
#include "cube.h"
#include "Texture.h"
#include "shader3d.h"

void MeshRenderer::Render(GraphicsDevice& device)
{
	// トランスフォーム取得
	Transform& tf = gameObject().transform();

	// ワールド行列計算
	DirectX::XMMATRIX mtxWorld =
		DirectX::XMMatrixScaling(tf.scale().x, tf.scale().y, tf.scale().z) *
		tf.rotation().ToXMMATRIX() *
		DirectX::XMMatrixTranslation(tf.position().x, tf.position().y, tf.position().z);

	// モデルが設定されている場合
	if (m_model)
	{
		// モデル描画
		ModelDraw(m_model, mtxWorld);
	}
	else if(m_mesh)
	{
		// メッシュ描画
		Shader3d_Begin();
		Shader3d_SetWorldMatrix(mtxWorld);
		Shader3d_SetMaterialDiffuse({ 1.0f, 1.0f, 1.0f, 1.0f });

		if (m_texture)
		{
			device.SetAlphaBlend(GraphicsDevice::BLEND_TRANSPARENT);
			m_texture->BindResource(device);
		}
		m_mesh->Render(device);
	}
	else
	{
		Cube_Draw(-1, mtxWorld);
	}
}
