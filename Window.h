
#ifndef WINDOW_H
#define WINDOW_H

#include <Windows.h>

struct WindowDesc
{
	const char* className = "DefaultWindowClass"; // ウィンドウクラス名
	const char* windowName = "Default Window";    // ウィンドウ名
	int width = 800;                                  // ウィンドウの幅
	int height = 600;                                 // ウィンドウの高さ
	DWORD style = WS_OVERLAPPEDWINDOW;                // ウィンドウスタイル
};


class Window
{
private:
	// ウィンドウハンドル
	HWND m_hWnd = nullptr;

	static constexpr const char* WINDOW_CLASS_NAME = "BasicWindowClass";

	// ウィンドウの幅と高さ
	int m_width = 0;
	int m_height = 0;

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
	{
		Window* window = nullptr;
		if (msg == WM_NCCREATE)
		{
			CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lp);
			window = reinterpret_cast<Window*>(createStruct->lpCreateParams);
			SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
			window->m_hWnd = hWnd;
		}
		else
		{
			window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
		}

		if (window)
		{
			return window->ProcessMessage(msg, wp, lp);
		}

		DefWindowProc(hWnd, msg, wp, lp);
	}

	static void RegisterWindowClass(const wchar_t* className, HINSTANCE hInst)
	{
		static bool isRegistered = false;
		if (isRegistered) return;

		//ウィンドウクラスの登録
		WNDCLASSEX wcex{};											// 構造体	{}があると無いとでは違う　ない場合はごみが入ってしまう　あると中身を初期化してくれる　WNDCLASSEX wcex = {};でもよい
		wcex.cbSize = sizeof(WNDCLASSEX);
		wcex.lpfnWndProc = WindowProc;									// ウィンドウプロシージャというものを登録しないといけない(関数ポインタ)
		wcex.hInstance = hInst;
		wcex.hIcon = LoadIcon(hInst, IDI_APPLICATION);			// どんなアイコンにするかを決めている
		wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);				// カーソルの絵を変えたりする場所
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);			// ウィンドウが出来た時の背景を変えることができる
		wcex.lpszMenuName = nullptr;								// メニューは作らない　本来はいらない上で初期化しているため
		wcex.lpszClassName = WINDOW_CLASS_NAME;							// ウィンドウクラス名でWINDOW_CLASSを引き出す
		wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);	// スモールアイコンを登録できる
		
	}
	void CreateWindowInstance(const WindowDesc& desc, HINSTANCE hInst)
	{
		//メインウィンドウの作成
		HWND hWnd = CreateWindow(					//hWndは返り値　ハンドル　識別子
			WINDOW_CLASS_NAME,
			desc.windowName,						//"あいうえお"でも良い
			WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME,	//ウィンドウスタイルを変えられるフラグ
			//window_style,						  	//フラグを管理して、最大化させないようにできる
			window_x,				  				//ウィンドウの初期座標Xを決める　CW_USEDEFAULTは適当な位置に設定してくれる
			window_y,							  	//ウィンドウの初期座標Yを決める
			window_width,						  	//ウィンドウの幅
			window_height,			  			  	//ウィンドウの高さ
			nullptr,	  						  	//親のウィンドウハンドルを作るとき　nullptrは親がいないため入れている
			nullptr,	  						  	//メニュー
			hInstance,
			nullptr
		);
	}
public:
	Window(const WindowDesc& desc, HINSTANCE hInstance)
		: m_width(desc.width), m_height(desc.height)
	{

	}
	virtual ~Window()
	{
		if(m_hWnd)
		{
			DestroyWindow(m_hWnd);
			m_hWnd = nullptr;
		}
	}

	HWND GetHWND() const { return m_hWnd; }
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

protected:
	// メッセージ処理メソッド
	virtual LRESULT ProcessMessage(UINT msg, WPARAM wp, LPARAM lp)
	{
		switch (msg)
		{
		case WM_SIZE: 
			m_width = LOWORD(lp); 
			m_height = HIWORD(lp); 
		return 0;

		case WM_DESTROY: 
			PostQuitMessage(0); 
			return 0;
		}

		return DefWindowProc(m_hWnd, msg, wp, lp);
	}

};


#endif
