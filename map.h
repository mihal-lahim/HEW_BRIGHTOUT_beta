/*==============================================================================

 ゴルフコースの制御 [map.h]
 Author : sumi rintarou
 Date :2025/11/04
--------------------------------------------------------------------------------

==============================================================================*/
//#ifndef MAP_H
//#define MAP_H
//
//#include <d3d11.h>
//#include <DirectXMath.h>
//#include <vector>
//
//// 地面タイル用構造体
//struct GroundTile
//{
//    DirectX::XMFLOAT3 position;
//    int textureId;
//    float width;
//    float depth;
//};
//
//class Map
//{
//private:
//    std::vector<GroundTile> m_GroundTiles;
//
//public:
//    Map() = default;
//    ~Map() = default;
//
//    void Initialize();
//    void Finalize();
//    void Draw() const;
//
//    // 電線ブロック取得用（仮のBlock型とPowerLine判定を追加）
//    struct Block {
//        enum Type {
//            PowerLine,
//            Other
//        };
//        Type type;
//        DirectX::XMFLOAT3 position;
//        Type GetType() const { return type; }
//        DirectX::XMFLOAT3 GetPosition() const { return position; }
//    };
//
//    // 電線上のブロックリスト取得
//    std::vector<Block> GetBlocks() const {
//        std::vector<Block> blocks;
//        // GroundTileから仮にPowerLineブロックを生成（実装は適宜修正）
//        for (const auto& tile : m_GroundTiles) {
//            // ここでは仮に全てPowerLineとして追加
//            blocks.push_back(Block{ Block::PowerLine, tile.position });
//        }
//        return blocks;
//    }
//};
//
//// グローバル Map インスタンス
//extern Map g_MapInstance;
//
//#endif // MAP_H

#ifndef MAP_H
#define MAP_H

#include "GameObject.h"
#include "MapCell.h"
#include <array>

// マップ全体を管理するクラス
class Map : public GameObject
{
public:
	// マップサイズ定義（3x3）
	static constexpr int MAP_WIDTH = 3;
	static constexpr int MAP_HEIGHT = 3;

	// 1マスの大きさ
	static constexpr float CELL_SIZE = 10.0f;

private:
	// MapCell の2次元配列
	std::array<std::array<MapCell*, MAP_WIDTH>, MAP_HEIGHT> m_Cells{};

public:
	Map(const DirectX::XMFLOAT3& position);
	virtual ~Map();

	// 更新
	virtual void Update(double elapsedTime) override;

	// 描画（全MapCellを描画）
	virtual void Draw() const override;

	// セル取得
	MapCell* GetCell(int x, int y) const;

	void RegisterCell(int x, int y, MapCell* cell);
};

#endif // MAP_H