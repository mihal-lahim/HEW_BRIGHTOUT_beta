
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
	m_Camera = new TPSCamera(this);


	// Controller設定
	auto* controller = AddComponent<Controller>();

	// プレイヤー用コマンドセット作成
	auto* commandSet = AddComponent<PlayerCommandSet>();

	// InputSystem設定
	m_InputSystem = AddComponent<InputSystem>(controller, commandSet);



	// Healthコンポーネント設定
	AddComponent<Health>(100.0f);

	// PlayerMovementコンポーネント設定
	m_Movement = AddComponent<PlayerMovement>(m_Camera);

	// PlayerMorphSystemコンポーネント設定
	m_MorphSystem = AddComponent<PlayerMorphSystem>();


	// PlayerStateMachineコンポーネント設定
	m_StateMachine = AddComponent<PlayerStateMachine>();


	// MeshRenderer設定
	m_MeshRenderer = AddComponent<MeshRenderer>();


	// コライダー設定
	auto* collider = AddComponent<Collider>(ColliderType::CAPSULE, Vector3{ 0.5f, 1.0f, 0.0f });

	// RigidBody設定
	m_RigidBody = AddComponent<RigidBody>(1.0f, Vector3{ 1.0f, 1.0f, 1.0f });
	m_RigidBody->AddCollider(collider);
}

void Player::Start()
{
	// 初期状態設定
	m_StateMachine->ChangeState(&PlayerStates::HumanIdle, *this);
}

void Player::Update()
{
	// 状態管理コンポーネント更新
	m_StateMachine->Update(*this);
}
