#ifndef PLAYER_STATE_H
#define PLAYER_STATE_H


#include "Component.h"

class Player;

// 抽象ステート基底クラス
class PlayerState
{
public:
	// デフォルトコンストラクタとデストラクタ
	PlayerState() = default;
	virtual ~PlayerState() = default;

	// ステート切り替え時の初期化処理
	virtual void Enter(Player& player);
	// ステート中の毎フレーム処理
	virtual void HandleInput(Player& player);
	virtual void Update(Player& player);
};

// ステートマシン管理クラス
class PlayerStateMachine : public Component
{
private:
	// 現在のステート
	PlayerState* m_CurrentState = nullptr;
public:

	PlayerStateMachine() = default;
	~PlayerStateMachine() = default;

	// ステート変更
	void ChangeState(PlayerState* newState, Player& player);

	// 現在フレーム更新
	void Update(Player& player);
};


#endif
