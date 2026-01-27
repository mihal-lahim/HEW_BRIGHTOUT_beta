#include "Audio.h"
#include <iostream>

AudioManager g_Audio;

AudioManager::AudioManager()
{
}

AudioManager::~AudioManager()
{
    Uninit();
}

// ===============================
// オーディオエンジン初期化
// ===============================
bool AudioManager::Init()
{
    // miniaudio のエンジンを初期化
    if (ma_engine_init(nullptr, &m_Engine) != MA_SUCCESS)
    {
        std::cout << "Audio Engine 初期化失敗\n";
        return false;
    }
    return true;
}

// ===============================
// オーディオエンジン解放
// ===============================
void AudioManager::Uninit()
{
    // BGM が再生中なら解放
    if (m_BGMPlaying)
    {
        ma_sound_uninit(&m_BGM);
        m_BGMPlaying = false;
    }

    // エンジン解放
    ma_engine_uninit(&m_Engine);
}

// ===============================
// 効果音(SE) 再生（ワンショット）
// ===============================
void AudioManager::PlaySE(const std::string& filePath, float volume)
{
    ma_sound* se = new ma_sound();

    if (ma_sound_init_from_file(
        &m_Engine,
        filePath.c_str(),
        0,
        nullptr,
        nullptr,
        se) != MA_SUCCESS)
    {
        std::cout << "SE 読み込み失敗: " << filePath << "\n";
        delete se;
        return;
    }

    ma_sound_set_volume(se, volume);

    //再生終了時に自動解放
    ma_sound_set_end_callback(se, OnSEFinished, nullptr);

    ma_sound_start(se);
}

// ===============================
// BGM 再生（ストリーミング）
// ===============================
bool AudioManager::PlayBGM(const std::string& filePath, bool loop)
{
    // すでにBGMが鳴っていたら停止＆解放
    if (m_BGMPlaying)
    {
        ma_sound_stop(&m_BGM);
        ma_sound_uninit(&m_BGM);
    }

    // BGM をストリーミング再生で読み込み
    if (ma_sound_init_from_file(
        &m_Engine,
        filePath.c_str(),
        MA_SOUND_FLAG_STREAM,   // BGMはストリーミング
        nullptr,
        nullptr,
        &m_BGM) != MA_SUCCESS)
    {
        std::cout << "BGM 読み込み失敗\n";
        return false;
    }

    // ループ設定
    ma_sound_set_looping(&m_BGM, loop ? MA_TRUE : MA_FALSE);

    // 再生開始
    ma_sound_start(&m_BGM);

    m_BGMPlaying = true;
    return true;
}

// ===============================
// BGM 停止
// ===============================
void AudioManager::StopBGM()
{
    if (!m_BGMPlaying) return;

    ma_sound_stop(&m_BGM);
    ma_sound_uninit(&m_BGM);
    m_BGMPlaying = false;
}

// ===============================
// マスター音量設定（全体）
// ===============================
void AudioManager::SetMasterVolume(float volume)
{
    ma_engine_set_volume(&m_Engine, volume);
}

// ===============================
// BGM 音量設定
// ===============================
void AudioManager::SetBGMVolume(float volume)
{
    if (m_BGMPlaying)
    {
        ma_sound_set_volume(&m_BGM, volume);
    }
}

// ===============================
// SE コールバック
// ===============================
void AudioManager::OnSEFinished(void*, ma_sound* pSound)
{
    if (pSound)
    {
        ma_sound_uninit(pSound);
        delete pSound;
    }
}