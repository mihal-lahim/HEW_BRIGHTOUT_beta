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
#include "direct3d.h"
#include "shader.h"
#include "shader3d.h"
#include "texture.h"
#include "cube.h"
#include "sprite.h"
#include "DebugOstream.h"
#include "DebugText.h"
#include "keyboard.h"
#include "mouse.h"
#include "KeyLogger.h"
#include "Audio.h"
#include "Game.h"
#include <sstream>
#include "EngineCore.h"

#include "light.h"


using namespace DirectX;

//=======================================
// ウィンドウ情報
//=======================================
static constexpr char WINDOW_CLASS[] = "GameWindow";	//メインウィンドウ名
static constexpr char TITLE[] = "ゲームウィンドウ";		//左上のタイトルバーのテキスト
// constexprはコンパイルをするときに定数化する
// constはビルド時に定数化する

//=======================================
// ウィンドウプロシージャ　プロトタイプ宣言
//=======================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


//=======================================
// メイン
//=======================================
int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hPrevInstance*/,
	_In_ LPSTR /*lpCmdLine*/, _In_ int nCmdShow)
{

	(void)CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	//DPIスケーリング
	SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	//ウィンドウクラスの登録
	WNDCLASSEX wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = WINDOW_CLASS;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	RegisterClassEx(&wcex);

	constexpr int window_Width = 1920;
	constexpr int window_Height = 1080;

	//クライアント領域のサイズを持った短形
	RECT window_rect = { 0,0,window_Width,window_Height };

	//ウィンドウのスタイル
	DWORD window_style = WS_EX_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX);
	//指定したクライアント領域を確保するために新たな短形座標を計算
	AdjustWindowRect(&window_rect, window_style, FALSE);

	//ウィンドウの幅と高さを算出
	int window_width = window_rect.right - window_rect.left;
	int window_height = window_rect.bottom - window_rect.top;

	//プライマリモニターの画面解像度取得
	int desktop_width = GetSystemMetrics(SM_CXSCREEN);
	int desktop_height = GetSystemMetrics(SM_CYSCREEN);

	// デスクトップの真ん中にウィンドウが生成されるように座標を計算
	// ただし万が一、デスクトップよりウィンドウが大きい場合は左上に表示
	int window_x = std::max((desktop_width - window_width) / 2, 0);
	int window_y = std::max((desktop_height - window_height) / 2, 0);

	//メインウィンドウの作成
	HWND hWnd = CreateWindowEx(
		0, 
		WINDOW_CLASS, 
		TITLE,
		window_style,
		window_x, 
		window_y,
		window_width, 
		window_height,
		nullptr, 
		nullptr, 
		hInstance, 
		nullptr
	);

	// システムタイマーの初期化
	SystemTimer_Initialize();

	// マウスの初期化
	Mouse_Initialize(hWnd);

	//マウスのカーソル
	Mouse_SetVisible(false);

	// キーロガーの初期化
	KeyLogger_Initialize();

	// オーディオの初期化
	InitAudio();

	ShowWindow(hWnd, nCmdShow);	//ウィンドウ表示
	UpdateWindow(hWnd);			//ウィンドウの描画の更新

	// Direct3Dの初期化
	if (!Direct3D_Initialize(hWnd))
	{
		PostQuitMessage(0);//メッセージを表示
		Direct3D_Finalize();
		Sprite_Finalize();

		return 0;//初期化に失敗したので終了
	}
	else
	{
		
		if (!Shader_Initialize(Direct3D_GetDevice(), Direct3D_GetContext()))
		{
			PostQuitMessage(0);//メッセージを表示
		}
		else
		{
			Texture_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());

			//スプライトの初期化
			Sprite_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());

			// キューブの初期化
			Cube_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());
		}
	}

	ShowWindow(hWnd, nCmdShow);	//ウィンドウ表示
	UpdateWindow(hWnd);			//ウィンドウの描画の更新

	Shader3d_Initialize(Direct3D_GetDevice(), Direct3D_GetContext());

	/*
	hal::DebugText dt(Direct3D_GetDevice(), Direct3D_GetContext(),
		L"texture/consolab_ascii_512.png",
		Direct3D_GetBackBufferWidth(), Direct3D_GetBackBufferHeight(),
		0.0f, 0.0f,
		0, 0,
		0.0f, 0.0f
	);
	*/

	Light_Initialize();

	XMFLOAT3 direction;
	{
		XMVECTOR dirVec = XMVectorSet(-1.0f, -1.5f, 1.0f, 0.0f);
		XMStoreFloat3(&direction, dirVec);
	}
	Light_SetDiffuse({ 0.7f,0.7f,0.7f }, direction);

	Light_SetAmbient({ 0.2f,0.1f,0.1f,1.0f });

	EngineCore engineCore{};
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

			/*
#if defined (DEBUG) || defined(_DEBUG)
				// FPSを表示する
				std::stringstream ss;
				ss << "fps:" << fps << std::endl;
				dt.SetText(ss.str().c_str());
				dt.Draw();	// FPSの描画
				dt.Clear();	// FPSのクリア
#endif
*/
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
	Direct3D_Finalize();    // Direct3Dの終了処理


	CoUninitialize();

	return (int)msg.wParam;	// メッセージのwParamを返す
}
//=======================================
// ウィンドウプロシージャ
//=======================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ACTIVATEAPP:
		Keyboard_ProcessMessage(message, wParam, lParam);
		Mouse_ProcessMessage(message, wParam, lParam);
		break;
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse_ProcessMessage(message, wParam, lParam);
		break;
	case WM_DESTROY:			//ウィンドウの破棄メッセージ　ここを外すとウィンドウを閉じれなくなるため必須
		PostQuitMessage(0);		//WM_QUITメッセージの送信
		break;

	case WM_CLOSE:// ウィンドウが閉じられるときに本当に閉じるかどうかの判別をする
		if (MessageBox(hWnd, "本当に閉じてもよろしいですか？", "確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK)
		{
			DestroyWindow(hWnd);//指定のウィンドウにWM_DESTROYメッセージを送る
		}
		break;//DefWindowProc関数にメッセージを流さず終了することによって何もなかったことにする
	case WM_KEYDOWN:// WM_KEYDOWNにはマウスのボタンやキーボードのキーが押されたときに受け取るメッセージ
		if (wParam == VK_ESCAPE)// wParamには押されたキーの仮想キーと呼ばれるキーコードが格納されている
		{
			SendMessage(hWnd, WM_CLOSE, 0, 0);// WM_CLOSEメッセージの送信
		}
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard_ProcessMessage(message, wParam, lParam);
		break;
	default:
		//通常のメッセージ処理はこの関数に任せる
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}




