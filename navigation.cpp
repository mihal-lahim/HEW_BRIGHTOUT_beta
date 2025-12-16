
#include "navigation.h"
#include <array>
#include <algorithm>

void NavMesh::Initialize(const NAVMESH_DESC& desc) noexcept
{
	// ユーザー定義の設定値をRecast用の設定値に変換する
	m_Cfg.cs = desc.cellsize;
	m_Cfg.ch = desc.cellheight;
	m_Cfg.walkableSlopeAngle = desc.walkableAngle;
	m_Cfg.walkableHeight = static_cast<int>(ceilf(desc.agentHeight / m_Cfg.ch));
	m_Cfg.walkableClimb = static_cast<int>(floorf(desc.walkableClimb / m_Cfg.ch));
	m_Cfg.walkableRadius = static_cast<int>(ceilf(desc.agentRadius / m_Cfg.cs));
	m_Cfg.maxEdgeLen = static_cast<int>(desc.maxEdgeLength / m_Cfg.cs);
	m_Cfg.maxSimplificationError = desc.maxSimplificationError;
	m_Cfg.minRegionArea = static_cast<int>(desc.minRegionArea * (m_Cfg.cs * m_Cfg.cs));
	m_Cfg.mergeRegionArea = static_cast<int>(desc.mergeRegionArea * (m_Cfg.cs * m_Cfg.cs));
	m_Cfg.maxVertsPerPoly = desc.maxVertsPerPoly;
	m_Cfg.detailSampleDist = desc.detailSampleDist;
	m_Cfg.detailSampleMaxError = desc.detailSampleMaxError;
}

bool NavMesh::Build() noexcept
{
	// AABB(最大座標と最小座標)の初期化
	std::array<float, 3> bmin{ FLT_MAX,FLT_MAX,FLT_MAX };
	std::array<float, 3> bmax{ -FLT_MAX,-FLT_MAX,-FLT_MAX };

	// 事前に用意した頂点配列（Recastはfloat*で頂点配列を受け取るため）
	std::vector<float> verts(m_Vertexes.size() * 3);

	// 頂点群からAABBを計算
	for (auto& vert : m_Vertexes)
	{
		bmin.at(0) = std::min<float>(bmin.at(0), vert.x);
		bmin.at(1) = std::min<float>(bmin.at(1), vert.y);
		bmin.at(2) = std::min<float>(bmin.at(2), vert.z);

		bmax.at(0) = std::max<float>(bmax.at(0), vert.x);
		bmax.at(1) = std::max<float>(bmax.at(1), vert.y);
		bmax.at(2) = std::max<float>(bmax.at(2), vert.z);

		// 事前に用意した頂点配列に頂点データをコピー
		verts.push_back(vert.x);
		verts.push_back(vert.y);
		verts.push_back(vert.z);
	}

	rcContext ctx{};

	// ハイトフィールドの幅と高さを計算
	int width = static_cast<int>((bmax.at(0) - bmin.at(0)) / m_Cfg.cs);
	int height = static_cast<int>((bmax.at(2) - bmin.at(2)) / m_Cfg.cs);
	
	// ハイトフィールドの作成
	if (!rcCreateHeightfield(&ctx, *m_Heightfield, width, height, &bmin.at(0), &bmax.at(0), m_Cfg.cs, m_Cfg.ch)) 
		return false;

	// 三角形のラスタライズ
	if(!rcRasterizeTriangles(&ctx, &verts.at(0), static_cast<int>(m_Vertexes.size()), 
		&m_Indexes.at(0), &RC_WALKABLE_AREA, static_cast<int>(m_Indexes.size() / 3), *m_Heightfield, m_Cfg.walkableClimb)) 
		return false;

	// コンパクトハイトフィールドの作成
	m_CompactHeightfield = rcAllocCompactHeightfield();
	if (!rcBuildCompactHeightfield(&ctx, m_Cfg.walkableHeight, m_Cfg.walkableClimb, *m_Heightfield, *m_CompactHeightfield))
		return false;

	// ウォーカブルエリアの最適化
	if (!rcErodeWalkableArea(&ctx, m_Cfg.walkableRadius, *m_CompactHeightfield))
		return false;

	// NavMesh領域の分割＆統合
	if(!rcBuildRegions(&ctx, *m_CompactHeightfield, 0, m_Cfg.minRegionArea, m_Cfg.mergeRegionArea))
		return false;

	// NavMeshの輪郭線の作成
	m_ContourSet = rcAllocContourSet();
	if (!rcBuildContours(&ctx, *m_CompactHeightfield, m_Cfg.maxSimplificationError, m_Cfg.maxEdgeLen, *m_ContourSet))
		return false;

	// ポリゴンメッシュの作成
	m_PolyMesh = rcAllocPolyMesh();
	if (!rcBuildPolyMesh(&ctx, *m_ContourSet, m_Cfg.maxVertsPerPoly, *m_PolyMesh))
		return false;

	// デティールメッシュの作成
	m_PolyMeshDetail = rcAllocPolyMeshDetail();
	if (!rcBuildPolyMeshDetail(&ctx, *m_PolyMesh, *m_CompactHeightfield, m_Cfg.detailSampleDist, m_Cfg.detailSampleMaxError, *m_PolyMeshDetail))
		return false;

	// NavMesh作成用パラメータの設定
	dtNavMeshCreateParams params{};
	params.verts = m_PolyMesh->verts;
	params.vertCount = m_PolyMesh->nverts;
	params.polys = m_PolyMesh->polys;
	params.polyFlags = m_PolyMesh->flags;
	params.polyAreas = m_PolyMesh->areas;
	params.polyCount = m_PolyMesh->npolys;
	params.nvp = m_PolyMesh->nvp;

	params.detailMeshes = m_PolyMeshDetail->meshes;
	params.detailVerts = m_PolyMeshDetail->verts;
	params.detailVertsCount = m_PolyMeshDetail->nverts;
	params.detailTris = m_PolyMeshDetail->tris;
	params.detailTriCount = m_PolyMeshDetail->ntris;

	params.walkableHeight = m_Cfg.walkableHeight * m_Cfg.ch;
	params.walkableRadius = m_Cfg.walkableRadius * m_Cfg.cs;
	params.walkableClimb = m_Cfg.walkableClimb * m_Cfg.ch;

	rcVcopy(params.bmin, m_PolyMesh->bmin);
	rcVcopy(params.bmax, m_PolyMesh->bmax);

	params.cs = m_PolyMesh->cs;
	params.ch = m_PolyMesh->ch;
	params.buildBvTree = true;



	// NavMeshのパラメータの作成
	unsigned char* navData = nullptr;
	int navDataSize = 0;
	if(!dtCreateNavMeshData(&params, &navData, &navDataSize))
		return false;


	// NavMeshの作成
	m_NavMesh = dtAllocNavMesh();
	if (m_NavMesh->init(navData, navDataSize, DT_TILE_FREE_DATA) != DT_SUCCESS)
		return false;


	// NavMeshQueryの作成
	m_NavMeshQuery = dtAllocNavMeshQuery();
	if (m_NavMeshQuery->init(m_NavMesh, 2048) != DT_SUCCESS)
		return false;


	return true;
}

void NavMesh::Clear() noexcept
{
	m_Cfg = rcConfig{};
	m_Heightfield = nullptr;
	m_CompactHeightfield = nullptr;
	m_ContourSet = nullptr;
	m_PolyMesh = nullptr;
	m_PolyMeshDetail = nullptr;
	m_NavMesh = nullptr;
	m_NavMeshQuery = nullptr;
}
