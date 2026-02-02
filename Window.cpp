#include "Window.h"
#include "WindowSystem.h"

LRESULT Window::WindowProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	Window* window = nullptr;

	// ウィンドウインスタンスの取得
	if (msg == WM_NCCREATE)
	{
		CREATESTRUCT* createStruct = reinterpret_cast<CREATESTRUCT*>(lp);
		window = reinterpret_cast<Window*>(createStruct->lpCreateParams);
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(window));
		window->m_hWnd = hWnd;
	}
	else
	{
		// 既存のウィンドウインスタンスを取得
		window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	}

	// 終了メッセージの処理
	if (WM_DESTROY == msg)
	{
		PostQuitMessage(0);
		return 0;
	}
	else if (WM_CLOSE == msg)
	{
		DestroyWindow(hWnd);
		return 0;
	}
	else if (WM_KEYDOWN)
	{
		if (wp == VK_ESCAPE)
		{
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
	}

	// メッセージの処理
	if (window)
	{
		return window->ProcessMessage(msg, wp, lp);
	}

	// デフォルトのメッセージ処理
	return DefWindowProc(hWnd, msg, wp, lp);
}

void Window::RegisterWindowClass(HINSTANCE hInstance)
{
	static bool isRegistered = false;
	if (isRegistered) return;

	// ウィンドウクラスの登録
	WNDCLASSEX wcex{};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.lpfnWndProc = WindowProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = kClassName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	RegisterClassEx(&wcex);
	isRegistered = true;
}

void Window::CreateWindowInstance(const WindowDesc& desc, HINSTANCE hInstance)
{
	// ウィンドウスタイルの設定
	DWORD style = WS_OVERLAPPEDWINDOW;

	// リサイズ不可、枠なし設定
	if (!desc.Resizable) style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX); 
	if (desc.Borderless) style = WS_POPUP;

	// クライアント領域のサイズを確保するためにウィンドウサイズを調整
	RECT rect = { 0, 0, desc.Width, desc.Height };
	AdjustWindowRect(&rect, style, FALSE);


	// ウィンドウの位置を計算
	int windowX = CW_USEDEFAULT;
	int windowY = CW_USEDEFAULT;

	// ウィンドウのサイズを計算
	int windowW = rect.right - rect.left;
	int windowH = rect.bottom - rect.top;

	// センター表示の設定
	if (desc.CenterScreen)
	{
		int screenW = GetSystemMetrics(SM_CXSCREEN);
		int screenH = GetSystemMetrics(SM_CYSCREEN);
		windowX = std::max((screenW - windowW) / 2, 0);
		windowY = std::max((screenH - windowH) / 2, 0);
	}

	// ウィンドウの作成
	m_hWnd = CreateWindowEx(
		0,
		kClassName,
		desc.WindowName.c_str(),
		style,
		windowX,
		windowY,
		windowW,
		windowH,
		nullptr,
		nullptr,
		hInstance,
		this
	);
}

Window::Window(const WindowDesc& desc, HINSTANCE hInstance)
	: m_width(desc.Width), m_height(desc.Height)
{
	RegisterWindowClass(hInstance);
	CreateWindowInstance(desc, hInstance);
}

Window::~Window()
{
	if (m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = nullptr;
	}
}

LRESULT Window::ProcessMessage(UINT msg, WPARAM wp, LPARAM lp)
{
	if (m_windowSystem)
	{
		m_windowSystem->ProcessMessage(this, msg, wp, lp);
	}
	return DefWindowProc(m_hWnd, msg, wp, lp);
}
