/*==============================================================================

   動画プレイヤーコントローラ [VideoPlayerController.h]
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef VIDEO_PLAYER_CONTROLLER_H
#define VIDEO_PLAYER_CONTROLLER_H

#include "ScriptComponent.h"
#include "MFVideoDecoder.h"
#include "VideoTexture.h"
#include "Texture.h"
#include <string>
#include <vector>

class MeshRenderer;

class VideoPlayerController : public ScriptComponent
{
public:
	// 動画ファイルパス（Start前に設定する）
	std::wstring videoPath;
	float transitionDelay = 1.0f;

	void Start() override;
	void Update() override;
	void OnDestroy() override;

private:
	MFVideoDecoder m_decoder;
	VideoTexture m_videoTexture;
	Texture m_wrapperTexture;

	MeshRenderer* m_meshRenderer = nullptr;
	GameObject* m_videoQuad = nullptr;
	bool m_initialized = false;
	bool m_transitionRequested = false;
	float m_elapsed = 0.0f;

	std::vector<std::string> m_preloadModelPaths{};
	std::vector<std::wstring> m_preloadTexturePaths{};
	size_t m_modelPreloadIndex = 0;
	size_t m_texturePreloadIndex = 0;
	bool m_preloadCompleted = false;

	void SetupPreloadQueue();
	void UpdatePreload();
};

#endif
