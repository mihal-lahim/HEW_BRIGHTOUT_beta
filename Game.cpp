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
	new PoleManager();

	// プレイヤーオブジェクトの生成
	Player* player = new Player();
	player->transform.Position = XMFLOAT3(0.0f, 10.0f, 0.0f);

	// 床オブジェクトの生成
	CubeObject* cube = new CubeObject();

	cube->transform.Position = XMFLOAT3(0.0f, -20.0f, 0.0f);
	cube->transform.Scale = XMFLOAT3(10.0f, 10.0f, 10.0f);
}
