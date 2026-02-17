#include "TitleController.h"
#include "GameObject.h"
#include "SceneSystem.h"
#include "InputSystem.h"
#include "Namioka.h"

void TitleController::Update()
{
	auto& pad = input().gamePad(0);

	// コントローラーのいずれかのボタンが押されたらゲームシーンへ遷移
	if (pad.IsConnected())
	{
		if (pad.IsDown(BUTTON_A) ||
			pad.IsDown(BUTTON_B) ||
			pad.IsDown(BUTTON_X) ||
			pad.IsDown(BUTTON_Y) ||
			pad.IsDown(BUTTON_START) ||
			pad.IsDown(BUTTON_BACK) ||
			pad.IsDown(BUTTON_LB) ||
			pad.IsDown(BUTTON_RB))
		{
			scene().ChangeScene<Namioka>();
			return;
		}
	}

	// キーボードのEnterキーでも遷移可能
	auto& kb = input().keyboard();
	if (kb.IsDown(KK_ENTER))
	{
		scene().ChangeScene<Namioka>();
		return;
	}
}
