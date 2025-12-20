#include "PowerLine.h"
#include "Pole.h"
#include "cube.h"
#include <cmath>


using namespace DirectX;

PowerLine::PowerLine(PoleID first, PoleID second)
	: m_ConnectedPoles({ first, second }), m_Length(0.0f)
{}


void PowerLine::Draw() const
{
	// 電柱の位置を取得
	if (m_PoleManager == nullptr) return;
	const auto& poles = m_PoleManager->GetPoles();
	const XMFLOAT3& pos1 = poles.at(m_ConnectedPoles.first)->GetTopPos();
	const XMFLOAT3& pos2 = poles.at(m_ConnectedPoles.second)->GetTopPos();

	// 電線の中点を計算
	XMFLOAT3 midPos{
		(pos1.x + pos2.x) * 0.5f,
		(pos1.y + pos2.y) * 0.5f,
		(pos1.z + pos2.z) * 0.5f
	};

	// 平行移動行列を計算
	XMMATRIX translation = XMMatrixTranslation(midPos.x, midPos.y, midPos.z);


	// 電線の方向ベクトルを計算
	XMVECTOR dir = XMVector3Normalize(XMLoadFloat3(&pos2) - XMLoadFloat3(&pos1));

	// 回転行列を計算
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX look = XMMatrixLookToLH(XMVectorZero(), dir, up);
	XMMATRIX rotation = XMMatrixInverse(nullptr, look);

	// スケーリング行列を計算
	XMMATRIX scale = XMMatrixScaling(0.05f, 0.05f, m_Length * 0.5f);

	// ワールド行列を計算
	XMMATRIX world = scale * rotation * translation;

	// 電線を描画
	Cube_Draw(0, world);
}
