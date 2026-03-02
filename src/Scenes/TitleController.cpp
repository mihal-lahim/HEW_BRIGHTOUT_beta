#include "TitleController.h"
#include "GameObject.h"
#include "SceneSystem.h"
#include "InputSystem.h"
#include "LoadingMovieScene.h"
#include "FadeController.h"

void TitleController::Start()
{
	// フェードコントローラを自分のGameObjectに追加
	m_fadeController = gameObject().AddComponent<FadeController>();

	// タイトル画面表示時にフェードインを開始
	m_fadeController->StartFadeIn(1.0f, [this]()
	{
		// フェードイン完了後に入力受付開始
		m_fadeReady = true;
	});
}

void TitleController::Update()
{
	// フェードイン完了前、または遷移中は入力を無視
	if (!m_fadeReady || m_isTransitioning) return;

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
			m_isTransitioning = true;
			m_fadeController->StartFadeOut(1.0f, [this]()
			{
				// フェードアウト完了後にシーン切り替え
				scene().ChangeScene<LoadingMovieScene>();
			});
			return;
		}
	}

	// キーボードのEnterキーでも遷移可能
	auto& kb = input().keyboard();
	if (kb.IsDown(KK_ENTER))
	{
		m_isTransitioning = true;
		m_fadeController->StartFadeOut(1.0f, [this]()
		{
			// フェードアウト完了後にシーン切り替え
			scene().ChangeScene<LoadingMovieScene>();
		});
		return;
	}
}
