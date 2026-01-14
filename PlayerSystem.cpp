#include "PlayerSystem.h"
#include "player_state.h"
#include "player.h"
#include "input_system.h"
#include "player_movement.h"
#include "player_morphsystem.h"

void PlayerSystem::Start()
{
	// 状態管理コンポーネント取得
	m_StateMachine = GetOwner()->GetComponent<PlayerStateMachine>();

	// 入力システム取得
	m_InputSystem = GetOwner()->GetComponent<InputSystem>();

	// 移動コンポーネント取得
	m_Movement = GetOwner()->GetComponent<PlayerMovement>();

	// 変身システム取得
	m_MorphSystem = GetOwner()->GetComponent<PlayerMorphSystem>();

	// メッシュレンダラー取得
	m_MeshRenderer = GetOwner()->GetComponent<MeshRenderer>();
}

void PlayerSystem::Update(double elapsedTime)
{
	m_StateMachine->Update(elapsedTime, *this);
}
