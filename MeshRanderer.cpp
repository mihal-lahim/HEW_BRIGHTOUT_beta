#include "MeshRanderer.h"
#include "model.h"
#include "GameObject.h"
#include "cube.h"

void MeshRenderer::Render()
{
	// トランスフォーム取得
	Transform* tf = &GetOwner()->Transform;

	// ワールド行列計算
	DirectX::XMMATRIX mtxWorld =
		DirectX::XMMatrixScaling(tf->Scale.x, tf->Scale.y, tf->Scale.z) *
		DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&tf->Rotation.Quat)) *
		DirectX::XMMatrixTranslation(tf->Position.x, tf->Position.y, tf->Position.z);

	// モデルが設定されている場合
	if (m_Model)
	{
		// モデル描画
		ModelDraw(m_Model, mtxWorld);
	}
	else
	{
		// モデルが設定されていない場合、テクスチャインデックスが有効ならCubeを描画
		if (m_TextureIndex >= 0)
		{
			Cube_Draw(m_TextureIndex, mtxWorld);
		}
		else
		{
			Cube_Draw(-1, mtxWorld);
		}
	}
}
