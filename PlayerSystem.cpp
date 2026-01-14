#include "PlayerSystem.h"
#include "player_state.h"
#include "player.h"

void PlayerSystem::Start()
{
	// 状態管理コンポーネント取得
	m_StateMachine = GetOwner()->GetComponent<PlayerStateMachine>();
}

void PlayerSystem::Update(double elapsedTime)
{
	m_StateMachine->Update(elapsedTime);
}
