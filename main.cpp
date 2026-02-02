//=======================================
// 
// Author	:角 凜太朗
// Date		:2025/10/16
//
//=======================================

#define WIN32_LEAN_AND_MEAN //古いウィンドウズのファイルを使わないように飛ばすため
#include<Windows.h>
#include <algorithm>
#include "SystemTimer.h"
#include "GraphicsDevice.h"
#include "shader.h"
#include "shader3d.h"
#include "texture.h"
#include "cube.h"
#include "sprite.h"
#include "mouse.h"
#include "KeyLogger.h"
#include "Audio.h"
#include "Game.h"
#include "EngineCore.h"
#include "Window.h"
#include "Model.h"
#include "DebugText.h"
#include "Time.h"
#include <sstream>

#include "light.h"


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
	Window* window = new Window(desc, hInstance);


	// システムタイマーの初期化
	SystemTimer_Initialize();

	// マウスの初期化
	Mouse_Initialize(window->GetHWND());

	//マウスのカーソル
	Mouse_SetVisible(false);

	// キーロガーの初期化
	KeyLogger_Initialize();

	// オーディオの初期化
	InitAudio();

	ShowWindow(window->GetHWND(), nCmdShow);	//ウィンドウ表示
	UpdateWindow(window->GetHWND());			//ウィンドウの描画の更新

	// Direct3Dの初期化
	if (!GetGraphicsDevice().Initialize(window->GetHWND()))
	{
		PostQuitMessage(0);//メッセージを表示
		GetGraphicsDevice().Finalize();
		Sprite_Finalize();

		return 0;//初期化に失敗したので終了
	}
	else
	{
		
		if (!Shader_Initialize(GetGraphicsDevice().GetDevice(), GetGraphicsDevice().GetDeviceContext()))
		{
			PostQuitMessage(0);//メッセージを表示
		}
		else
		{
			Texture_Initialize(GetGraphicsDevice().GetDevice(), GetGraphicsDevice().GetDeviceContext());

			//スプライトの初期化
			Sprite_Initialize(GetGraphicsDevice().GetDevice(), GetGraphicsDevice().GetDeviceContext());

			// キューブの初期化
			Cube_Initialize(GetGraphicsDevice().GetDevice(), GetGraphicsDevice().GetDeviceContext());
		}
	}

	ShowWindow(window->GetHWND(), nCmdShow);	//ウィンドウ表示
	UpdateWindow(window->GetHWND());			//ウィンドウの描画の更新

	Shader3d_Initialize(GetGraphicsDevice().GetDevice(), GetGraphicsDevice().GetDeviceContext());
	
	ModelInitialize(&GetGraphicsDevice());

	GameContext ctx = engineCore.GetGameContext();

	// ウィンドウをウィンドウシステムに登録
	ctx.windowSystem->RegisterWindow(window);
	ctx.renderingSystem->SetDevice(&GetGraphicsDevice());


	Light_Initialize();

	XMFLOAT3 direction;
	{
		XMVECTOR dirVec = XMVectorSet(-1.0f, -1.5f, 1.0f, 0.0f);
		XMStoreFloat3(&direction, dirVec);
	}
	Light_SetDiffuse({ 0.3f,0.28f,0.28f }, direction);

	Light_SetAmbient({ 0.55f,0.55f,0.55f,1.0f });


	engineCore.GetGameContext().sceneSystem->ChangeScene<Game>();

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
	Light_Finalize();
	UninitAudio();          // オーディオの終了処理
	Mouse_Finalize();       // マウスの終了処理
	Shader3d_Finalize();    // シェーダー3dの終了処理
	Cube_Finalize();        // キューブの終了処理
	Sprite_Finalize();      // スプライトの終了処理
	Texture_Finalize();     // テクスチャの終了処理
	Shader_Finalize();      // シェーダーの終了処理
	GetGraphicsDevice().Finalize();    // Direct3Dの終了処理


	CoUninitialize();

	return (int)msg.wParam;	// メッセージのwParamを返す
}
