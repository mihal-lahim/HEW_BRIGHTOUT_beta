
#ifndef PLAYER_MORPHSYSTEM_H
#define PLAYER_MORPHSYSTEM_H

#include "ScriptComponent.h"
#include "PoleManager.h"

class Player;

class PlayerMorphSystem : public ScriptComponent
{
private:
	// PoleManagerへの参照
	PoleManager* m_PoleManager = nullptr;
	// 変身可能範囲
	float m_MorphRadius = 5.0f;
public:
	void Awake() override;

	// 変身可能かどうかを判定する
	bool CanMorph();

	// 最寄りの電線IDを取得する
	PowerLineID GetNearestPowerLineID();
};


#endif
