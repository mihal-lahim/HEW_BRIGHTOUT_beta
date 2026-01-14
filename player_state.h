#ifndef PLAYER_STATE_H
#define PLAYER_STATE_H

#include "GameObject.h"
#include "Component.h"

class Player;

// 抽象ステート基底クラス
class PlayerState : public Component
{
public:
	// デフォルトコンストラクタとデストラクタ
	PlayerState() { SetActive(false); };
	virtual ~PlayerState() = default;

	// ステート切り替え時の初期化処理
	virtual void Enter();
	// ステート中の毎フレーム処理
	virtual void HandleInput();
	virtual void Update(double elapsedTime);
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
	void ChangeState(PlayerState* newState);

	// 現在フレーム更新
	void Update(double elapsedTime);
};


#endif
