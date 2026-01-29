#include "Game.h"
#include "Player.h"
#include "PoleManager.h"
#include "GameManager.h"
#include "Camera.h"
#include "CubeObject.h"
#include "DebugCamera.h"

using namespace DirectX;

void Game::Initialize()
{
	// PoleManagerオブジェクトの生成
	//new PoleManager();

	// プレイヤーオブジェクトの生成
	new DebugCamera();

	// 床オブジェクトの生成
	CubeObject* cube = new CubeObject();

	cube->transform.Position = Vector3(0.0f, -20.0f, 0.0f);
	cube->transform.Scale = Vector3(100.0f, 1.0f, 100.0f);
}
