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
	Instantiate(new PoleManager());

	// プレイヤーオブジェクトの生成
	Instantiate(new Player());

	// 床オブジェクトの生成
	CubeObject* cube = Instantiate(new CubeObject());

	cube->transform.Position = XMFLOAT3(0.0f, -5.0f, 0.0f);
	cube->transform.Scale = XMFLOAT3(50.0f, 1.0f, 50.0f);
}
