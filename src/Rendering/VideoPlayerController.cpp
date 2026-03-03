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
#include "Model.h"
#include <Windows.h>

void VideoPlayerController::Start()
{
	OutputDebugStringA("[VideoPlayerController] Start()\n");

	m_transitionRequested = false;
	m_elapsed = 0.0f;
	m_modelPreloadIndex = 0;
	m_texturePreloadIndex = 0;
	m_preloadCompleted = false;
	SetupPreloadQueue();

	// 動画ファイルを開く
	if (!m_decoder.Open(videoPath))
	{
		OutputDebugStringA("[VideoPlayerController] Failed to open video - continue loading without movie\n");
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
		OutputDebugStringA("[VideoPlayerController] Failed to create video texture - continue loading without movie\n");
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

	UpdatePreload();

	if (!m_transitionRequested && m_preloadCompleted && m_elapsed >= transitionDelay)
	{
		m_transitionRequested = true;
		OutputDebugStringA("[VideoPlayerController] Preload completed. ChangeScene<Namioka>()\n");
		scene().ChangeScene<Namioka>();
	}
}

void VideoPlayerController::OnDestroy()
{
	OutputDebugStringA("[VideoPlayerController] OnDestroy()\n");
	m_decoder.Shutdown();
	m_initialized = false;
}

void VideoPlayerController::SetupPreloadQueue()
{
	m_preloadModelPaths = {
		"model/sky.fbx",
		"model/intersection.glb",
		"model/cube.glb",
		"model/house.glb",
		"model/convenience.glb",
		"model/mansion01.glb",
		"model/mansion02.glb",
		"model/karaoke.glb",
		"model/apartment.glb",
		"model/clocktower01.glb",
		"model/wacdonald.glb",
		"model/powerplant.glb",
		"model/powerplant_broken.glb",
		"model/dentyuu.glb",
		"model/special_densen.glb",
		"model/special_dentyu.glb",
		"model/building01.glb",
		"model/building02.glb",
		"model/P_taiki.fbx",
		"model/run_01.fbx",
		"model/run_02.fbx",
		"model/run_03.fbx",
		"model/run_04.fbx",
		"model/P_kougeki_01.fbx",
		"model/P_kougeki_02.fbx",
		"model/E_kougeki_01.fbx",
		"model/E_kougeki_02.fbx",
		"model/E_kougeki_03.fbx",
		"model/E_kougeki_04.fbx",
		"model/E_kougeki_05.fbx",
		"model/E_run_01.fbx",
		"model/E_run_02.fbx",
		"model/E_run_03.fbx",
		"model/E_run_04.fbx",
		"model/E_run_05.fbx",
		"model/E_HP0_01.fbx",
		"model/E_HP0_02.fbx",
		"model/E_HP0_03.fbx"
	};

	m_preloadTexturePaths = {
		L"texture/ball.png",
		L"texture/kiru_effects_1.png"
	};
}

void VideoPlayerController::UpdatePreload()
{
	if (m_preloadCompleted)
	{
		return;
	}

	if (m_modelPreloadIndex < m_preloadModelPaths.size())
	{
		const auto& modelPath = m_preloadModelPaths[m_modelPreloadIndex++];
		gameObject().resource().Load<Model>(modelPath);
		std::string log = "[VideoPlayerController] Preload model: " + modelPath + "\n";
		OutputDebugStringA(log.c_str());
		return;
	}

	if (m_texturePreloadIndex < m_preloadTexturePaths.size())
	{
		const auto& texturePath = m_preloadTexturePaths[m_texturePreloadIndex++];
		gameObject().resource().Load<Texture>(texturePath);
		std::string log = "[VideoPlayerController] Preload texture index: " + std::to_string(m_texturePreloadIndex) + "\n";
		OutputDebugStringA(log.c_str());
		return;
	}

	m_preloadCompleted = true;
	OutputDebugStringA("[VideoPlayerController] Preload completed\n");
}
