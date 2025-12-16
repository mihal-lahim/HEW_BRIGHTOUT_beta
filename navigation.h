


#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "Recast.h"

#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourCrowd.h"

#include <vector>
#include <DirectXMath.h>
#include "debug_ostream.h"


// NavMeshの設定構造体
struct NAVMESH_DESC
{
	// セルの設定。小さくすればするほどクオリティの高いナビゲーションができるが、生成コストがその分かかる
	float cellsize = 0.3f;   // セルのサイズ（縦横の長さ）
	float cellheight = 0.2f; // セルの高さ

	// エージェントの能力の設定値
	float walkableAngle = 45.0f; // エージェントが歩行できる最大の傾斜角度
	float walkableClimb = 2.0f; // エージェントが歩行で登れる最大の段差高さ
	float agentHeight = 0.9f;   // エージェントの高さ（通れる最大の天井高）
	float agentRadius = 0.6f;   // エージェントの半径（通れる最大の幅）

	// NavMeshの生成に関する設定値
	float maxEdgeLength = 12;		  // Meshの辺の許容される最大の長さ（大きいほどMeshの輪郭が大雑把になる）
	float maxSimplificationError = 1.3f; // Meshの簡略化の許容される最大誤差（大きいほどMeshの輪郭が大雑把になる）
	float minRegionArea = 8;		  // メッシュ上の領域の最小サイズ（小さいほど細かい領域が生成され、孤島などが生成される可能性が上がる）
	float mergeRegionArea = 20;	      // メッシュ上の領域をマージする際の最小サイズ（閾値）（小さいほど細かい領域が維持される）
	int   maxVertsPerPoly = 6;        // ポリゴンあたりの最大頂点数（多いほどメッシュの自由度が上がるが、処理コストも上がる）

	// ディテールメッシュの生成に関する設定値（ディテールメッシュとは高さ情報を追加するためのもの）
	float detailSampleDist = 6.0f;     // ディテールメッシュのサンプリング距離（小さいほど詳細な高さ情報が得られるが、生成コストが上がる）
	float detailSampleMaxError = 1.0f; // ディテールメッシュの最大許容誤差（小さいほど詳細な高さ情報が得られるが、生成コストが上がる）
};

class NavMesh
{
private:
	rcConfig m_Cfg{};
	rcHeightfield* m_Heightfield = nullptr;
	rcCompactHeightfield* m_CompactHeightfield = nullptr;
	rcContourSet* m_ContourSet = nullptr;
	rcPolyMesh* m_PolyMesh = nullptr;
	rcPolyMeshDetail* m_PolyMeshDetail = nullptr;
	dtNavMesh* m_NavMesh = nullptr;
	dtNavMeshQuery* m_NavMeshQuery = nullptr;

	std::vector<DirectX::XMFLOAT3> m_Vertexes{};
	std::vector<unsigned short> m_Indexes{};
public:

	// コンストラクタと初期化関数
	NavMesh(const NAVMESH_DESC& desc) noexcept { Initialize(desc); }
	void Initialize(const NAVMESH_DESC& desc) noexcept;


	// NavMeshにメッシュを追加するテンプレート関数
	template<typename T>
		requires requires(T t) 
	{ { t.position } -> std::same_as<DirectX::XMFLOAT3>; } // TがDirectX::XMFLOAT3型のpositionメンバを持つことを要求
	bool AddMesh(const std::vector<T>& vertexes, const std::vector<unsigned int> indexes,const DirectX::XMMATRIX& worldmtx) noexcept
	{
		// 頂点数とインデックス数が一致しない場合は追加できない
		if (vertexes.size() != indexes.size())
		{
			hal::dout << "NavMesh::AddMesh: 頂点数とインデックス数が一致しません。" << std::endl;
			return false;
		}

		// 受け取った頂点配列をNavMesh用の頂点配列に変換して追加する
		m_Vertexes.reserve(m_Vertexes.size() + vertexes.size());
		for (auto& vert : vertexes)
		{
			DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(vert.position);
			vec = DirectX::XMVector3Transform(vec, worldmtx);

			DirectX::XMFLOAT3 pos;
			DirectX::XMStoreFloat3(&pos, vec);

			m_Vertexes.push_back(pos);
		}

		// 受け取ったインデックス配列をNavMesh用のインデックス配列に変換して追加する
		m_Indexes.reserve(m_Indexes.size() + indexes.size());
		unsigned int offset = m_Indexes.at(m_Indexes.size() - 1) + 1;
		for (auto& index : indexes)
		{
			m_Indexes.push_back(static_cast<unsigned short>(index + offset));
		}

		return true;
	}

	// NavMeshのビルドを行う関数
	bool Build() noexcept;

	// NavMeshのリセットを行う関数
	void Clear() noexcept;
};


class AgentManager
{

};





#endif