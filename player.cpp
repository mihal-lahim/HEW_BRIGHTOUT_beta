
#include "player.h"

#include "player_state_human_idle.h"
#include "player_state_human_walk.h"
#include "player_state_human_midair.h"
#include "player_state_electric.h"

Player::Player()
{
	// PlayerSystem設定
	AddComponent<PlayerSystem>();

	// TPSCamera作成
	GameObject* cam = GetOwner()->Create<GameObject>();
	Camera* camcomp = cam->AddComponent<Camera>();
	cam->AddComponent<TPSCamera>(this);



	// Controller設定
	auto* controller = AddComponent<Controller>();

	// InputSystem設定
	AddComponent<InputSystem>(controller);



	// Healthコンポーネント設定
	AddComponent<Health>(100.0f);

	// PlayerMovementコンポーネント設定
	AddComponent<PlayerMovement>(camcomp);

	// PlayerMorphSystemコンポーネント設定
	AddComponent<PlayerMorphSystem>();

	// 各種ステート作成
	auto* idle = AddComponent<PlayerState_Human_Idle>();
	AddComponent<PlayerState_Human_Walk>();
	AddComponent<PlayerState_Human_MidAir>();
	AddComponent<PlayerState_Electric>();

	// PlayerStateMachineコンポーネント設定
	AddComponent<PlayerStateMachine>(idle);


}