#pragma once
#include <string>
#include "miniaudio.h"


class AudioManager
{
public:
    AudioManager();
    ~AudioManager();

    // オーディオエンジン初期化 / 解放
    bool Init();
    void Uninit();

    // 2D 効果音（UI音など）
    void PlaySE(
        const std::string& filePath,
        float volume = 1.0f
    );

    // BGM 再生（ストリーミング）
    bool PlayBGM(
        const std::string& filePath,
        bool loop = true
    );

    // BGM 停止
    void StopBGM();

    // マスター音量（全体）
    void SetMasterVolume(float volume);

    // BGM 音量
    void SetBGMVolume(float volume);

	// 効果音終了コールバック
    static void OnSEFinished(void* pUserData, ma_sound* pSound);

private:
    ma_engine m_Engine{};   // オーディオエンジン
    ma_sound  m_BGM{};          // BGM サウンド
    bool      m_BGMPlaying{};   // 再生中フラグ
};

extern AudioManager g_Audio;