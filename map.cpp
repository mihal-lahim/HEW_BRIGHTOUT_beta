/*==============================================================================

 マップ制御 [map.cpp] - Block を道路・草地・電線・電柱・建物に変更
 Author : sumi rintarou
 Date :2025/11/04
--------------------------------------------------------------------------------

==============================================================================*/
//#include "map.h"
//#include "cube.h"
//#include "texture.h"
//
//using namespace DirectX;
//
//// グローバル Map インスタンス定義
//Map g_MapInstance;
//
//void Map::Initialize()
//{
//	Finalize();
//
//	// 地面用テクスチャ読込
//	int groundTex[4] = {
//		Texture_Load(L"texture/map_hidariue.png"),
//		Texture_Load(L"texture/map_migiue.png"),
//		Texture_Load(L"texture/map_hidarishita.png"),
//		Texture_Load(L"texture/map_migishita.png")
//	};
//
//	// 原点中心に4つのタイル配置
//	float tileSize = 25.0f; // タイルサイズを大きくする
//	m_GroundTiles.clear();
//	m_GroundTiles.reserve(4);
//
//	// 左上
//	m_GroundTiles.emplace_back(GroundTile{ {-tileSize * 0.5f, 0.0f, tileSize * 0.5f}, groundTex[0], tileSize, tileSize });
//	// 右上
//	m_GroundTiles.emplace_back(GroundTile{ {tileSize * 0.5f, 0.0f, tileSize * 0.5f}, groundTex[1], tileSize, tileSize });
//	// 左下
//	m_GroundTiles.emplace_back(GroundTile{ {-tileSize * 0.5f, 0.0f, -tileSize * 0.5f}, groundTex[2], tileSize, tileSize });
//	// 右下
//	m_GroundTiles.emplace_back(GroundTile{ {tileSize * 0.5f, 0.0f, -tileSize * 0.5f}, groundTex[3], tileSize, tileSize });
//}
//
//void Map::Finalize()
//{
//	m_GroundTiles.clear();
//}
//
//void Map::Draw() const
//{
//	for (const auto& tile : m_GroundTiles) {
//		// 地面は厚さ 0.1f でスケール
//		// キューブは中心から上下に0.05f ずつ広がるため、底面がY=0になるようにオフセット
//		XMMATRIX mtxScale = XMMatrixScaling(tile.width, 0.1f, tile.depth);
//		XMMATRIX mtxTrans = XMMatrixTranslation(tile.position.x, tile.position.y - 0.05f, tile.position.z);
//
//		XMMATRIX world = mtxScale * mtxTrans;
//		Cube_Draw(tile.textureId, world);
//	}
//}
//




#include "Map.h"

Map::Map(const DirectX::XMFLOAT3& position)
	: GameObject(position)
{
	// 3x3 の MapCell を生成
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			DirectX::XMFLOAT3 cellPos =
			{
				m_Position.x + x * CELL_SIZE,
				m_Position.y,
				m_Position.z + y * CELL_SIZE
			};

			m_Cells[y][x] = new MapCell(
				x,
				y,
				CELL_SIZE,
				cellPos
			);
		}
	}
}

Map::~Map()
{
	// MapCell の解放
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			delete m_Cells[y][x];
			m_Cells[y][x] = nullptr;
		}
	}
}

void Map::Update(double elapsedTime)
{
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			if (m_Cells[y][x])
			{
				m_Cells[y][x]->Update(elapsedTime);
			}
		}
	}
}

void Map::Draw() const
{
	for (int y = 0; y < MAP_HEIGHT; ++y)
	{
		for (int x = 0; x < MAP_WIDTH; ++x)
		{
			if (m_Cells[y][x])
			{
				m_Cells[y][x]->Draw();
			}
		}
	}
}

MapCell* Map::GetCell(int x, int y) const
{
	if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
	{
		return nullptr;
	}
	return m_Cells[y][x];
}

void Map::RegisterCell(int x, int y, MapCell* cell)
{
	// 範囲チェック
	if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
	{
		return;
	}

	// 既に登録されていたら上書き（仕様次第）
	m_Cells[y][x] = cell;
}
