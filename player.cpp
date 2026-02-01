
#include "Player.h"
#include "MeshRenderer.h"
#include "InputSystem.h"
#include "model.h"
#include "Camera.h"
#include "PhysicsBody.h"
#include "ColliderShape.h"

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


	// プレイヤー用コマンドセット作成
	auto* commandSet = gameObject().AddComponent<PlayerCommandSet>();

	// InputSystem設定
	inputHandler = gameObject().AddComponent<InputHandler>(&input().gamePad(), commandSet);



	// Healthコンポーネント設定
	gameObject().AddComponent<Health>(100.0f);


	auto* temp = gameObject().AddComponent<Camera>();

	// PlayerMovementコンポーネント設定
	movement = gameObject().AddComponent<PlayerMovement>(temp);

	// PlayerMorphSystemコンポーネント設定
	morphSystem = gameObject().AddComponent<PlayerMorphSystem>();


	// PlayerStateMachineコンポーネント設定
	stateMachine = gameObject().AddComponent<PlayerStateMachine>();


	// MeshRenderer設定
	meshRenderer = gameObject().AddComponent<MeshRenderer>();


	// ColliderShape設定
	CapsuleColliderDesc shapeDesc{};
	shapeDesc.Radius = 0.5f;
	shapeDesc.Height = 1.8f;
	CapsuleColliderShape shape{ shapeDesc };


	// PhysicsBody設定
	PhysicsBodyDesc bodyDesc{};
	bodyDesc.Mass = 1.0f;
	bodyDesc.Type = BodyType::DYNAMIC;
	bodyDesc.FixedRotation = { 1.0f, 0.0f, 1.0f };
	physicsBody = gameObject().AddComponent<PhysicsBody>(bodyDesc);
	physicsBody->AddShape(shape);
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
