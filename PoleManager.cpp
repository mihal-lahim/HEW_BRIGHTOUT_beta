#include "PoleManager.h"
#include "PowerLine.h"
#include "Pole.h"

using namespace DirectX;

PoleID PoleManager::RegisterPole(Pole* pole)
{
	// 電柱を登録し、そのIDを返す
	m_Poles.push_back(pole);
    return static_cast<PoleID>(m_Poles.size() - 1);
}

PowerLineID PoleManager::RegisterPowerLine(PowerLine* line)
{
	// 電線を登録
	m_PowerLines.push_back(line);

	// 接続されている電柱にも電線のIDを登録
	Pole* pole1 = m_Poles.at(line->GetPoles().first);
	Pole* pole2 = m_Poles.at(line->GetPoles().second);
	pole1->SetPowerLine((PoleID)m_PowerLines.size() - 1);
	pole2->SetPowerLine((PoleID)m_PowerLines.size() - 1);

	// 電柱の座標を取得
	XMVECTOR pos1 = XMLoadFloat3(&pole1->GetPosition());
	XMVECTOR pos2 = XMLoadFloat3(&pole2->GetPosition());

	// 電線の長さを設定
	line->SetLength(XMVectorGetX(XMVector3Length(pos1 - pos2)));

	return (PoleID)(m_PowerLines.size() - 1);
}

float PoleManager::GetPowerLineLength(PowerLineID id) const { return m_PowerLines.at(id)->GetLength(); }

PowerLineID PoleManager::GetPowerLineID(PoleID start, PoleID dest) const
{
	// 指定された2つの電柱を結ぶ電線を検索
	for (PowerLineID lineID : m_Poles.at(start)->GetLines())
	{
		// 電線を取得
		PowerLine* line = m_PowerLines.at(lineID);

		// 接続されている電柱のIDペアを取得
		auto [firstPole, secondPole] = line->GetPoles();

		// 電柱の組み合わせが一致するか確認
		if ((firstPole == start && secondPole == dest) || (firstPole == dest && secondPole == start))
			return lineID;
	}
	
	// 見つからなかった場合は-1を返す
	return -1;
}

DirectX::XMFLOAT3 PoleManager::GetPositionOnPowerLine(PoleID start, PoleID dest, float t) const
{
	// 電柱の頂点位置を取得
	XMVECTOR startVec = XMLoadFloat3(&m_Poles.at(start)->GetTopPos());
	XMVECTOR destVec = XMLoadFloat3(&m_Poles.at(dest)->GetTopPos());
		
	// 線形補間で電線上の位置を取得
	XMFLOAT3 result{};
	XMStoreFloat3(&result, XMVectorLerp(startVec, destVec, t));
	
	return result;
}

PoleID PoleManager::GetDirectionalPole(PoleID from, const XMVECTOR& direction) const
{
	// 指定された方向ベクトルを正規化
	XMVECTOR dirVec = XMVector3Normalize(direction);
	float maxDot = -1.0f;
	PoleID bestPole = -1;

	// 接続されている電柱をすべて走査
	for (PoleID lineID : m_Poles.at(from)->GetLines())
	{
		// その電線が接続しているもう一方の電柱を取得
		PowerLine* line = m_PowerLines.at(lineID);
		PoleID destID = (line->GetPoles().first == from) ? line->GetPoles().second : line->GetPoles().first;

		// 方向ベクトルとの内積を計算
		XMVECTOR destPos = XMLoadFloat3(&m_Poles.at(destID)->GetTopPos());
		XMVECTOR fromPos = XMLoadFloat3(&m_Poles.at(from)->GetTopPos());
		XMVECTOR toOtherVec = XMVector3Normalize(destPos - fromPos);
		float dot = XMVectorGetX(XMVector3Dot(dirVec, toOtherVec));

		// 最大の内積を持つ電柱を記録
		if (dot > maxDot)
		{
			maxDot = dot;
			bestPole = destID;
		}
	}

	return bestPole;
}
