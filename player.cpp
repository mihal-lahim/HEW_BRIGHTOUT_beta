
#include "player.h"
#include "MeshRenderer.h"
#include "Collider.h"
#include "RigidBody.h"
#include "player_command_set.h"
#include "model.h"

#include "player_state_human_idle.h"
#include "player_state_human_walk.h"
#include "player_state_human_midair.h"
#include "player_state_electric.h"

using namespace DirectX;

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

	// プレイヤー用コマンドセット作成
	auto* commandSet = AddComponent<PlayerCommandSet>();

	// InputSystem設定
	AddComponent<InputSystem>(controller, commandSet);



	// Healthコンポーネント設定
	AddComponent<Health>(100.0f);

	// PlayerMovementコンポーネント設定
	AddComponent<PlayerMovement>(camcomp);

	// PlayerMorphSystemコンポーネント設定
	AddComponent<PlayerMorphSystem>();


	// PlayerStateMachineコンポーネント設定
	AddComponent<PlayerStateMachine>(PlayerStates::HumanIdle);

	// コライダー設定
	auto* collider = AddComponent<Collider>(ColliderType::CAPSULE, XMFLOAT3{ 0.5f, 1.0f, 0.0f });

	// RigidBody設定
	auto* rigidbody = AddComponent<RigidBody>(1.0f, XMFLOAT3{ 1.0f, 0.0f, 1.0f });
	rigidbody->AddCollider(collider);


	// MeshRenderer設定
	AddComponent<MeshRenderer>();
}