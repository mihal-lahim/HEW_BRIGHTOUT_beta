
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
	AddComponent<InputSystem>(controller, commandSet);



	// Healthコンポーネント設定
	AddComponent<Health>(100.0f);

	// PlayerMovementコンポーネント設定
	AddComponent<PlayerMovement>(m_Camera);

	// PlayerMorphSystemコンポーネント設定
	AddComponent<PlayerMorphSystem>();


	// PlayerStateMachineコンポーネント設定
	AddComponent<PlayerStateMachine>(nullptr);


	// MeshRenderer設定
	AddComponent<MeshRenderer>();


	// コライダー設定
	auto* collider = AddComponent<Collider>(ColliderType::BOX, XMFLOAT3{ 1.0f, 1.0f, 1.0f });

	// RigidBody設定
	m_RigidBody = AddComponent<RigidBody>(1.0f, XMFLOAT3{ 1.0f, 1.0f, 1.0f });
	m_RigidBody->AddCollider(collider);
}

void Player::Start()
{
	// 状態管理コンポーネント取得
	m_StateMachine = GetComponent<PlayerStateMachine>();

	// 入力システム取得
	m_InputSystem = GetComponent<InputSystem>();

	// 移動コンポーネント取得
	m_Movement = GetComponent<PlayerMovement>();

	// 変身システム取得
	m_MorphSystem = GetComponent<PlayerMorphSystem>();

	// メッシュレンダラー取得
	m_MeshRenderer = GetComponent<MeshRenderer>();


	m_StateMachine->ChangeState(&PlayerStates::HumanIdle, *this);
}

void Player::Update()
{
	// 状態管理コンポーネント更新
	m_StateMachine->Update(*this);
}
