#ifndef PLAYER_STATE_H
#define PLAYER_STATE_H

#include "GameObject.h"


class Player;
class PlayerSystem;

// 抽象ステート基底クラス
class PlayerState
{
public:
	// デフォルトコンストラクタとデストラクタ
	PlayerState() = default;
	virtual ~PlayerState() = default;

	// ステート切り替え時の初期化処理
	virtual void Enter(PlayerSystem& playerSystem);
	// ステート中の毎フレーム処理
	virtual void HandleInput(PlayerSystem& playerSystem);
	virtual void Update(double elapsedTime, PlayerSystem& playerSystem);
};

// ステートマシン管理クラス
class PlayerStateMachine : public Component
{
private:
	// 現在のステート
	PlayerState* m_CurrentState = nullptr;
public:

	PlayerStateMachine(PlayerState* state)
		: m_CurrentState(state)
	{}
	~PlayerStateMachine() = default;

	// ステート変更
	void ChangeState(PlayerState* newState, PlayerSystem& playerSystem);

	// 現在フレーム更新
	void Update(double elapsedTime, PlayerSystem& playerSystem);
};


#endif
