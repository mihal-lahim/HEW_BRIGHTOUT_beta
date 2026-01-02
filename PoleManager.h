
#ifndef POLE_MANAGER_H
#define POLE_MANAGER_H

#include "GameObject.h"
#include <DirectXMath.h>
#include <vector>

class Pole;
class PowerLine;

typedef int PoleID;
typedef int PowerLineID;

class PoleManager : public GameObject
{
private:
	// 管理する電柱と電線のリスト
	std::vector<Pole*> m_Poles;
	std::vector<PowerLine*> m_PowerLines;
public:

	PoleManager() = default;
	virtual ~PoleManager() = default;

	// 電柱と電線の登録メソッド
	PoleID RegisterPole(Pole* pole);
	PowerLineID RegisterPowerLine(PowerLine* line);

	// 電線の長さ取得メソッド
	float GetPowerLineLength(PowerLineID id) const;
	// 電線ID取得メソッド
	PowerLineID GetPowerLineID(PoleID start, PoleID dest) const;
	// 電線上の位置取得メソッド 
	// t: 0.0f ~ 1.0f　の範囲を指定することで、今現在の電線上の位置を取得できる
	DirectX::XMFLOAT3 GetPositionOnPowerLine(PoleID start, PoleID dest, float t) const;
	// 指定した方向に最も合う、接続されている電柱のIDを取得するメソッド
	PoleID GetDirectionalPole(PoleID from, const DirectX::XMVECTOR& direction) const;

	// 電柱取得メソッド
	Pole* GetPole(PoleID id) const { return m_Poles.at(id); };
	// 電線取得メソッド
	PowerLine* GetPowerLine(PowerLineID id) const { return m_PowerLines.at(id); };
	// 電柱リスト取得メソッド
	std::vector<Pole*> GetPoles() const { return m_Poles; }
	// 電線リスト取得メソッド
	std::vector<PowerLine*> GetPowerLines() const { return m_PowerLines; }
};



#endif
