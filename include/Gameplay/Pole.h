
#ifndef POLE_H
#define POLE_H

#include <DirectXMath.h>
#include "ScriptComponent.h"
#include <vector>
#include "PoleManager.h"
#include "GameObject.h"


class Pole : public ScriptComponent
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
public:
    // コンストラクタ
    Pole(float height = 4.0f)
		: m_Height(height)
	{}
    virtual ~Pole() = default;

	// 電柱ID設定・取得メソッド
	PoleID GetID() const { return m_ID; }

	// 接続されている電線の追加メソッド
	void SetPowerLine(PowerLineID lineID) { m_ConnectedLines.push_back(lineID); }

	// 電柱の頂点位置取得メソッド
	Vector3 GetTopPos() const 
	{ return Vector3{ gameObject().transform().position().x , gameObject().transform().position().y + m_Height, gameObject().transform().position().z}; }

	// 接続されている電線リスト取得メソッド
	std::vector<PowerLineID> GetLines() const { return m_ConnectedLines; }


	friend class PoleManager;
};



#endif
