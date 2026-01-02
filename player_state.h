#ifndef PLAYER_STATE_H
#define PLAYER_STATE_H

class Player;

// 抽象ステート基底クラス
class PlayerState
{
public:
	// デフォルトコンストラクタとデストラクタ
	PlayerState() = default;
	virtual ~PlayerState() = default;

	// ステート切り替え時の初期化処理
	virtual void Enter(Player&) {}
	// ステート中の毎フレーム処理
	virtual void HandleInput(Player&) {}
	virtual void Update(Player&, double) {}
	virtual void Draw(const Player&) const {}
	// ステート離脱時の処理
	virtual void Exit(Player&) {}

	// コピーコンストラクタと代入演算子を削除（ステートの複製を防止）
	PlayerState(const PlayerState&) = delete;
	PlayerState& operator=(const PlayerState&) = delete;
};

// ステートマシン管理クラス
class PlayerStateMachine
{
private:
	// 現在のステート
	PlayerState* m_CurrentState = nullptr;
public:

	PlayerStateMachine() = default;
	~PlayerStateMachine() = default;

	// ステート変更
	void ChangeState(Player& player, PlayerState* newState);

	// 現在フレーム更新
	void Update(Player& player, double elapsedTime);
	void Draw(const Player& player) const;
};


#endif
