
#include "Player.h"
#include "MeshRenderer.h"
#include "Collider.h"
#include "RigidBody.h"
#include "model.h"
#include "GameManager.h"

using namespace DirectX;

Player::Player()
{
	//m_HumanModel = ModelLoad("model/kirby.fbx");
}

Player::~Player()
{
	//ModelRelease(m_HumanModel);
}

void Player::Awake()
{
	// TPSCamera作成
	camera = new TPSCamera(this);


	// Controller設定
	auto* controller = AddComponent<XinputController>();

	// プレイヤー用コマンドセット作成
	auto* commandSet = AddComponent<PlayerCommandSet>();

	// InputSystem設定
	inputSystem = AddComponent<InputSystem>(controller, commandSet);



	// Healthコンポーネント設定
	AddComponent<Health>(100.0f);

	// PlayerMovementコンポーネント設定
	movement = AddComponent<PlayerMovement>(camera);

	// PlayerMorphSystemコンポーネント設定
	morphSystem = AddComponent<PlayerMorphSystem>();


	// PlayerStateMachineコンポーネント設定
	stateMachine = AddComponent<PlayerStateMachine>();


	// MeshRenderer設定
	meshRenderer = AddComponent<MeshRenderer>();


	// コライダー設定
	auto* collider = AddComponent<Collider>(ColliderType::CAPSULE, Vector3{ 0.5f, 1.0f, 0.0f });

	// RigidBody設定
	rigidBody = AddComponent<RigidBody>(1.0f, Vector3{ 1.0f, 1.0f, 1.0f });
	rigidBody->AddCollider(collider);
}

void Player::Start()
{
	// 初期状態設定
	stateMachine->ChangeState(&PlayerStates::HumanIdle, *this);
}

void Player::Update()
{
	// 状態管理コンポーネント更新
	stateMachine->Update(*this);
}
