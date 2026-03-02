/*==============================================================================

   Media Foundation MP4動画デコーダ [MFVideoDecoder.h]
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef MF_VIDEO_DECODER_H
#define MF_VIDEO_DECODER_H

#include <string>
#include <cstdint>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <wrl/client.h>

class MFVideoDecoder
{
public:
	MFVideoDecoder() = default;
	~MFVideoDecoder();

	// 動画ファイルを開く
	bool Open(const std::wstring& path);

	// 時間を進めてフレームを更新する
	void Update(float deltaTime);

	// 現在のフレームデータを取得する（RGB32形式）
	// outData: フレームデータへのポインタ, outStride: 行あたりのバイト数
	bool GetFrame(uint8_t*& outData, uint32_t& outStride);

	// 動画が終了したかどうか
	bool IsFinished() const;

	// 先頭にシークする（ループ再生用）
	void SeekToStart();

	// 終了処理
	void Shutdown();

	// 動画の幅と高さ
	uint32_t GetWidth() const { return m_width; }
	uint32_t GetHeight() const { return m_height; }

private:
	Microsoft::WRL::ComPtr<IMFSourceReader> m_sourceReader;

	uint32_t m_width = 0;
	uint32_t m_height = 0;
	bool m_isFinished = false;
	bool m_hasFrame = false;

	// 現在のフレームデータ
	Microsoft::WRL::ComPtr<IMFSample> m_currentSample;
	Microsoft::WRL::ComPtr<IMFMediaBuffer> m_currentBuffer;
	uint8_t* m_lockedData = nullptr;
	uint32_t m_lockedStride = 0;
	bool m_locked2D = false;  // Lock2Dでロックされたかどうか

	// フレーム時間管理
	int64_t m_currentTime = 0;     // 100ナノ秒単位
	int64_t m_frameDuration = 0;   // 1フレームあたりの時間
	double m_accumulator = 0.0;    // 蓄積時間（秒）

	// 前回のフレームバッファをアンロックする
	void UnlockCurrentBuffer();

	// 次のフレームを読み込む
	bool ReadNextFrame();
};

#endif
