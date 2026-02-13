//=======================================
// 
// Author	:角 凜太朗
// Date		:2025/10/16
//
//=======================================

#define WIN32_LEAN_AND_MEAN //古いウィンドウズのファイルを使わないように飛ばすため
#include<Windows.h>
#include "GraphicsDevice.h"
#include "Mouse.h"
#include "Game.h"
#include "EngineCore.h"
#include "Window.h"

#include "Kageyama.h"
#include "Kasiwagi.h"
#include "Namioka.h"

using namespace DirectX;


//=======================================
// メイン
//=======================================
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/,
	_In_ LPSTR /*lpCmdLine*/, _In_ int nCmdShow)
{
	(void)CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	//DPIスケーリング
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	// EngineCoreの生成
	EngineCore engineCore{};

	// ウィンドウの作成
	WindowDesc desc{};
	desc.Width = 1920;
	desc.Height = 1080;
	desc.Resizable = false;
	desc.Borderless = true;
	desc.CenterScreen = true;
	desc.Fullscreen = false;
	Window window = Window(desc, hInstance);
	GraphicsDevice graphicsDevice = GraphicsDevice{};

	// マウスの初期化
	Mouse_Initialize(window.GetHWND());

	//マウスのカーソル
	Mouse_SetVisible(false);

	ShowWindow(window.GetHWND(), nCmdShow);	//ウィンドウ表示
	UpdateWindow(window.GetHWND());			//ウィンドウの描画の更新

	// Direct3Dの初期化
	if (!graphicsDevice.Initialize(window.GetHWND()))
	{
		PostQuitMessage(0);//メッセージを表示
		graphicsDevice.Finalize();

		return 0;//初期化に失敗したので終了
	}

	ShowWindow(window.GetHWND(), nCmdShow);	//ウィンドウ表示
	UpdateWindow(window.GetHWND());			//ウィンドウの描画の更新


	GameContext ctx = engineCore.GetGameContext();

	// ウィンドウをウィンドウシステムに登録
	ctx.windowSystem->RegisterWindow(&window);
	ctx.renderingSystem->Initialize(graphicsDevice);


	// デフォルトシーン
	//engineCore.GetGameContext().sceneSystem->ChangeScene<Game>();



	//engineCore.GetGameContext().sceneSystem->ChangeScene<Kageyama>();


	//engineCore.GetGameContext().sceneSystem->ChangeScene<Kasiwagi>();


	engineCore.GetGameContext().sceneSystem->ChangeScene<Namioka>();


	//ゲームループ
	MSG msg{};
	do {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))// ウィンドウメッセージが来ていたら
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}


		engineCore.Update();



		} while (msg.message != WM_QUIT);

	// 終了処理
	Mouse_Finalize();       // マウスの終了処理
	graphicsDevice.Finalize(); // Direct3Dの終了処理


	CoUninitialize();

	return (int)msg.wParam;	// メッセージのwParamを返す
}
