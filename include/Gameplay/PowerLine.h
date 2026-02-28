

#ifndef POWER_LINE_H
#define POWER_LINE_H

#include <DirectXMath.h>
#include "ScriptComponent.h"
#include "PoleManager.h"
#include <utility>

class PowerLine : public ScriptComponent
{
private:
	// 電線ID
	PowerLineID m_ID = -1;
	// 所有者の PoleManager へのポインタ
	PoleManager* m_PoleManager = nullptr;
	// 接続されている電柱のIDペア
	std::pair<PoleID, PoleID> m_ConnectedPoles;
	// 電線の長さ
	float m_Length = 0.0f;
public:
    PowerLine(PoleID first, PoleID second);
    virtual ~PowerLine() = default;

	// 電線ID設定・取得メソッド
	PowerLineID GetID() const { return m_ID; }


	// 接続されている電柱IDペア取得メソッド
	void SetPoles(PoleID first, PoleID second) { m_ConnectedPoles = { first, second }; }

	// 接続されている電柱IDペア取得メソッド
	std::pair<PoleID, PoleID> GetPoles() const { return m_ConnectedPoles; }

	// 長さ設定メソッド
	void SetLength(float length) { m_Length = length; }

	// 長さ取得メソッド
	float GetLength() const { return m_Length; }

	// 電線ベクトル取得メソッド
	Vector3 GetLineVector() const;

	friend class PoleManager;
};

#endif
