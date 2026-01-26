#include "Game.h"
#include "Player.h"
#include "PoleManager.h"
#include "GameManager.h"
#include "Camera.h"
#include "CubeObject.h"

using namespace DirectX;

void Game::Initialize()
{
	// PoleManagerオブジェクトの生成
	//Instantiate(new PoleManager());

	// プレイヤーオブジェクトの生成
	//Instantiate(new Player());

	// カメラオブジェクトの生成
	Camera* camera = Instantiate<Camera>(new Camera());

	camera->transform.Position = { 0.0f, 0.0f, -10.0f };
	camera->transform.Rotation = Quaternion::Identity();

	// キューブオブジェクトの生成
	Instantiate(new CubeObject());
}
