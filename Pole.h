
#ifndef POLE_H
#define POLE_H

#include <DirectXMath.h>
#include "GameObject.h"
#include <vector>
#include "PoleManager.h"


class Pole : public GameObject
{
private:
	// 電柱ID
	PoleID m_ID = -1;  // 電柱ID
	// 所有者の PoleManager へのポインタ
    PoleManager* m_PoleManager = nullptr;
    // 接続されている電線のリスト
	std::vector<PowerLineID> m_ConnectedLines;
    // 電柱の高さ
    float m_Height;
	// 電柱の頂点位置
    DirectX::XMFLOAT3 m_TopPos{};
public:
    // コンストラクタ
    Pole(const DirectX::XMFLOAT3& pos, float height = 4.0f);
    virtual ~Pole() = default;

	// 電柱ID設定・取得メソッド
	void SetID(PoleID id) { m_ID = id; }
	PoleID GetID() const { return m_ID; }

	// 所有者の PoleManager 設定メソッド
	void SetOwner(PoleManager* manager) { m_PoleManager = manager; }
	// 接続されている電線の追加メソッド
	void SetPowerLine(PowerLineID lineID) { m_ConnectedLines.push_back(lineID); }
	// 電柱の高さ設定メソッド
	void SetHeight(float height)
	{ 
		m_Height = height;
		m_TopPos = DirectX::XMFLOAT3(Transform.Position.x, Transform.Position.y + m_Height, Transform.Position.z);
	}
	// 電柱の頂点位置取得メソッド
	const DirectX::XMFLOAT3& GetTopPos() const { return m_TopPos; }
	// 接続されている電線リスト取得メソッド
	std::vector<PowerLineID> GetLines() const { return m_ConnectedLines; }
};



#endif
