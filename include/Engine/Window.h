
#ifndef WINDOW_H
#define WINDOW_H

#include <Windows.h>
#include <string>

class WindowSystem;

struct WindowDesc
{
	// ウィンドウ名
	std::string WindowName = "Default Window";

	// ウィンドウの幅
	int Width = 1280;

	// ウィンドウの高さ
	int Height = 720;

	// リサイズ可能かどうか
	bool Resizable = true;

	// 枠なしウィンドウかどうか
	bool Borderless = false;

	// 画面中央に表示するかどうか
	bool CenterScreen = true;

	// フルスクリーン表示にするかどうか
	bool Fullscreen = false;
};


class Window
{
public:
	Window(const WindowDesc& desc, HINSTANCE hInstance);
	virtual ~Window();

	// ウィンドウハンドルの取得
	HWND GetHWND() const { return m_hWnd; }

	// ウィンドウの幅と高さの取得
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }

protected:
	// メッセージ処理メソッド
	LRESULT ProcessMessage(UINT msg, WPARAM wp, LPARAM lp);

private:
	// ウィンドウシステムへのポインタ
	WindowSystem* m_windowSystem = nullptr;

	// ウィンドウハンドル
	HWND m_hWnd = nullptr;

	// ウィンドウクラス名
	static constexpr const char kClassName[] = "BasicWindowClass";

	// ウィンドウの幅と高さ
	int m_width = 0;
	int m_height = 0;

	// ウィンドウプロシージャ
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	// ウィンドウクラスの登録
	static void RegisterWindowClass(HINSTANCE hInstance);

	// ウィンドウの作成
	void CreateWindowInstance(const WindowDesc& desc, HINSTANCE hInstance);

	friend class WindowSystem;
};


#endif
