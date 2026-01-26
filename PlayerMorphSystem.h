
#ifndef PLAYER_MORPHSYSTEM_H
#define PLAYER_MORPHSYSTEM_H

#include "Component.h"
#include "PoleManager.h"

class Player;

class PlayerMorphSystem : public Component
{
private:
	// PoleManagerへの参照
	PoleManager* m_PoleManager = nullptr;
public:
	void Awake() override;

	// 変身可能かどうかを判定する
	bool CanMorph();

	// 最寄りの電線IDを取得する
	PowerLineID GetNearestPowerLineID();
};


#endif
