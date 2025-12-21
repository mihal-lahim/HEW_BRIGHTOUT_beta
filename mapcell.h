/*==============================================================================

   マップセルクラス[mapcell.h]
														 Author : syota
														 Date   : 2025/12/20
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef MAPCELL_H
#define MAPCELL_H

#include "GameObject.h"

// マップを構成する1マス分のクラス
class MapCell : public GameObject
{
private:
	int m_CellX = 0; // マップ上のXインデックス
	int m_CellY = 0; // マップ上のYインデックス
	float m_CellSize = 1.0f; // 1マスのサイズ

public:
	MapCell(int cellX, int cellY, float cellSize,
		const DirectX::XMFLOAT3& position,
		MODEL* model = nullptr,
		int textureId = -1);

	virtual ~MapCell() = default;

	// 更新
	virtual void Update(double elapsedTime) override;

	// 描画
	virtual void Draw() const override;

	// セル情報取得
	int GetCellX() const { return m_CellX; }
	int GetCellY() const { return m_CellY; }
	float GetCellSize() const { return m_CellSize; }
};

#endif // MAPCELL_H
