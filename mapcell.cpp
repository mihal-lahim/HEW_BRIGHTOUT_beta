/*==============================================================================

   マップセルクラス[mapcell.cpp]
														 Author : syota
														 Date   : 2025/12/20
--------------------------------------------------------------------------------

==============================================================================*/

#include "mapcell.h"

// コンストラクタ
MapCell::MapCell(
	int cellX,
	int cellY,
	float cellSize,
	const DirectX::XMFLOAT3& position,
	MODEL* model,
	int textureId)
	: GameObject(position, model, textureId)
	, m_CellX(cellX)
	, m_CellY(cellY)
	, m_CellSize(cellSize)
{
	// 必要ならスケールをセルサイズに合わせる
	m_Scale = { cellSize, 1.0f, cellSize };
}

void MapCell::Update(double elapsedTime)
{

}

void MapCell::Draw() const
{
	if (!m_IsActive) return;

	// モデル
	if (m_pModel)
	{
		// ※ 既存の描画関数
	}
	// テクスチャ
	else if (m_TextureId >= 0)
	{
		// ※ Sprite描画など
	}
}