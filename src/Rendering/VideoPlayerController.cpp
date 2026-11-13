/*==============================================================================

   動画プレイヤーコントローラ [VideoPlayerController.cpp]
--------------------------------------------------------------------------------

==============================================================================*/
#include "VideoPlayerController.h"
#include "GameObject.h"
#include "Renderer.h"
#include "RenderingSystem.h"
#include "SceneSystem.h"
#include "UIDrawer.h"
#include "GameTime.h"
#include "Namioka.h"
#include <Windows.h>

void VideoPlayerController::Start()
{
	OutputDebugStringA("[VideoPlayerController] Start()\n");

	m_transitionRequested = false;
	m_elapsed = 0.0f;

	// 動画ファイルを開く
	if (!m_decoder.Open(videoPath))
	{
		OutputDebugStringA("[VideoPlayerController] Failed to open video - will skip to next scene\n");
		m_initialized = false;
		return;
	}

	uint32_t width = m_decoder.GetWidth();
	uint32_t height = m_decoder.GetHeight();

	OutputDebugStringA("[VideoPlayerController] Video opened: ");
	OutputDebugStringA((std::to_string(width) + "x" + std::to_string(height) + "\n").c_str());

	// 動的テクスチャを作成
	GraphicsDevice& device = gameObject().rendering().GetGraphicsDevice();
	if (!m_videoTexture.CreateBuffer(device, width, height))
	{
		OutputDebugStringA("[VideoPlayerController] Failed to create video texture - will skip to next scene\n");
		m_decoder.Shutdown();
		m_initialized = false;
		return;
	}

	// 最初のフレームを動的テクスチャに書き込む
	uint8_t* frameData = nullptr;
	uint32_t stride = 0;
	if (m_decoder.GetFrame(frameData, stride))
	{
		m_videoTexture.UpdateFromFrame(device, frameData, stride);
	}

	// SRVをラッパーTextureに設定（AddRefで参照カウントを維持）
	ID3D11ShaderResourceView* srv = m_videoTexture.GetSRV();
	if (srv)
	{
		srv->AddRef();
		m_wrapperTexture.CreateFromLoaded(device, srv, width, height);
	}

	// フルスクリーンUI四角形を作成
	m_videoQuad = UI::CreateUI(
		&gameObject(),
		L"texture/Default.png",  // ダミー（後で上書きする）
		Vector3(0.0f, 0.0f, 0.0f),
		Vector3(1920.0f, 1080.0f, 1.0f),
		"UiVS.cso",
		"UiPS.cso"
	);

	if (m_videoQuad)
	{
		m_meshRenderer = m_videoQuad->GetComponent<MeshRenderer>();
		if (m_meshRenderer)
		{
			m_meshRenderer->renderQueue = RenderQueue::UI;
			// ラッパーTextureを設定（自動ロードを防止）
			m_meshRenderer->material.texture = &m_wrapperTexture;
		}
	}

	m_initialized = true;
	OutputDebugStringA("[VideoPlayerController] Initialized successfully\n");
}

void VideoPlayerController::Update()
{
	float deltaTime = static_cast<float>(Time::DeltaTime());
	m_elapsed += deltaTime;

	// 動画再生中の処理
	if (m_initialized)
	{
		// デコーダを更新
		m_decoder.Update(deltaTime);

		// 動画終了時はループ再生
		if (m_decoder.IsFinished())
		{
			OutputDebugStringA("[VideoPlayerController] Looping video\n");
			m_decoder.SeekToStart();
		}

		// フレームデータを取得して動的テクスチャに書き込む
		uint8_t* frameData = nullptr;
		uint32_t stride = 0;
		if (m_decoder.GetFrame(frameData, stride))
		{
			GraphicsDevice& device = gameObject().rendering().GetGraphicsDevice();
			m_videoTexture.UpdateFromFrame(device, frameData, stride);
		}
	}

	// 初期化失敗時も含め、遷移タイマーは常に動作する
	if (!m_transitionRequested && m_elapsed >= transitionDelay)
	{
		m_transitionRequested = true;
		OutputDebugStringA("[VideoPlayerController] ChangeScene<Namioka>()\n");
		scene().ChangeScene<Namioka>();
	}
}

void VideoPlayerController::OnDestroy()
{
	OutputDebugStringA("[VideoPlayerController] OnDestroy()\n");
	m_decoder.Shutdown();
	m_initialized = false;
}
