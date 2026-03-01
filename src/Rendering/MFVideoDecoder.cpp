/*==============================================================================

   Media Foundation MP4動画デコーダ [MFVideoDecoder.cpp]
--------------------------------------------------------------------------------

==============================================================================*/
#include "MFVideoDecoder.h"
#include <mferror.h>
#include <Windows.h>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

MFVideoDecoder::~MFVideoDecoder()
{
	Shutdown();
}

bool MFVideoDecoder::Open(const std::wstring& path)
{
	HRESULT hr = S_OK;

	// SourceReader を作成
	Microsoft::WRL::ComPtr<IMFAttributes> attributes;
	hr = MFCreateAttributes(&attributes, 1);
	if (FAILED(hr))
	{
		OutputDebugStringA("[MFVideoDecoder] MFCreateAttributes failed\n");
		return false;
	}

	// ビデオ処理（フォーマット変換）を有効にする
	hr = attributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);
	if (FAILED(hr))
	{
		OutputDebugStringA("[MFVideoDecoder] SetUINT32(ENABLE_VIDEO_PROCESSING) failed\n");
		return false;
	}

	hr = MFCreateSourceReaderFromURL(path.c_str(), attributes.Get(), &m_sourceReader);
	if (FAILED(hr))
	{
		OutputDebugStringA("[MFVideoDecoder] MFCreateSourceReaderFromURL failed\n");
		return false;
	}

	// 音声ストリームを無効化
	hr = m_sourceReader->SetStreamSelection(
		static_cast<DWORD>(MF_SOURCE_READER_ALL_STREAMS), FALSE);
	if (FAILED(hr))
	{
		OutputDebugStringA("[MFVideoDecoder] SetStreamSelection(ALL, FALSE) failed\n");
		return false;
	}

	hr = m_sourceReader->SetStreamSelection(
		static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM), TRUE);
	if (FAILED(hr))
	{
		OutputDebugStringA("[MFVideoDecoder] SetStreamSelection(VIDEO, TRUE) failed\n");
		return false;
	}

	// 出力フォーマットをRGB32に設定
	Microsoft::WRL::ComPtr<IMFMediaType> outputType;
	hr = MFCreateMediaType(&outputType);
	if (FAILED(hr))
	{
		OutputDebugStringA("[MFVideoDecoder] MFCreateMediaType failed\n");
		return false;
	}

	hr = outputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
	if (FAILED(hr)) return false;

	hr = outputType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
	if (FAILED(hr)) return false;

	hr = m_sourceReader->SetCurrentMediaType(
		static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM),
		nullptr, outputType.Get());
	if (FAILED(hr))
	{
		OutputDebugStringA("[MFVideoDecoder] SetCurrentMediaType(RGB32) failed\n");
		return false;
	}

	// 実際に設定されたメディアタイプから解像度を取得
	Microsoft::WRL::ComPtr<IMFMediaType> currentType;
	hr = m_sourceReader->GetCurrentMediaType(
		static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM),
		&currentType);
	if (FAILED(hr))
	{
		OutputDebugStringA("[MFVideoDecoder] GetCurrentMediaType failed\n");
		return false;
	}

	UINT32 width = 0, height = 0;
	hr = MFGetAttributeSize(currentType.Get(), MF_MT_FRAME_SIZE, &width, &height);
	if (FAILED(hr))
	{
		OutputDebugStringA("[MFVideoDecoder] MFGetAttributeSize failed\n");
		return false;
	}

	m_width = width;
	m_height = height;

	// フレームレートを取得してフレーム間隔を計算
	UINT32 numerator = 0, denominator = 0;
	hr = MFGetAttributeRatio(currentType.Get(), MF_MT_FRAME_RATE, &numerator, &denominator);
	if (SUCCEEDED(hr) && numerator > 0)
	{
		// 100ナノ秒単位でのフレーム間隔
		m_frameDuration = static_cast<int64_t>(
			(10000000.0 * static_cast<double>(denominator)) / static_cast<double>(numerator));
	}
	else
	{
		// デフォルト30fps
		m_frameDuration = 333333;
	}

	m_isFinished = false;
	m_hasFrame = false;
	m_currentTime = 0;
	m_accumulator = 0.0;

	// 最初のフレームを読み込む
	ReadNextFrame();

	OutputDebugStringA("[MFVideoDecoder] Open succeeded\n");
	return true;
}

void MFVideoDecoder::Update(float deltaTime)
{
	if (m_isFinished || !m_sourceReader)
		return;

	m_accumulator += static_cast<double>(deltaTime);

	// フレーム間隔（秒）
	double frameDurationSec = static_cast<double>(m_frameDuration) / 10000000.0;

	// 蓄積時間がフレーム間隔を超えたら次のフレームへ
	while (m_accumulator >= frameDurationSec)
	{
		m_accumulator -= frameDurationSec;

		UnlockCurrentBuffer();

		if (!ReadNextFrame())
		{
			// 動画終了
			m_isFinished = true;
			break;
		}
	}
}

bool MFVideoDecoder::GetFrame(uint8_t*& outData, uint32_t& outStride)
{
	if (!m_hasFrame || !m_currentBuffer)
	{
		outData = nullptr;
		outStride = 0;
		return false;
	}

	// まだロックされていなければロックする
	if (!m_lockedData)
	{
		DWORD maxLength = 0;
		DWORD currentLength = 0;

		// IMF2DBuffer を試す（ストライドの正確な取得のため）
		Microsoft::WRL::ComPtr<IMF2DBuffer> buffer2D;
		HRESULT hr = m_currentBuffer.As(&buffer2D);
		if (SUCCEEDED(hr))
		{
			LONG stride = 0;
			hr = buffer2D->Lock2D(&m_lockedData, &stride);
			if (SUCCEEDED(hr))
			{
				m_locked2D = true;

				// RGB32のボトムアップ形式の場合、strideが負になることがある
				if (stride < 0)
				{
					m_lockedStride = static_cast<uint32_t>(-stride);
					// ポインタを先頭行（メモリ先頭）に調整（ボトムアップ→トップダウン変換）
					m_lockedData = m_lockedData + (static_cast<LONG>(m_height) - 1) * stride;
				}
				else
				{
					m_lockedStride = static_cast<uint32_t>(stride);
				}

				outData = m_lockedData;
				outStride = m_lockedStride;
				return true;
			}
		}

		// フォールバック：通常のLock
		hr = m_currentBuffer->Lock(&m_lockedData, &maxLength, &currentLength);
		if (FAILED(hr))
		{
			m_lockedData = nullptr;
			outData = nullptr;
			outStride = 0;
			return false;
		}
		m_locked2D = false;
		m_lockedStride = m_width * 4; // RGB32 = 4バイト/ピクセル
	}

	outData = m_lockedData;
	outStride = m_lockedStride;
	return true;
}

bool MFVideoDecoder::IsFinished() const
{
	return m_isFinished;
}

void MFVideoDecoder::SeekToStart()
{
	if (!m_sourceReader)
		return;

	UnlockCurrentBuffer();

	PROPVARIANT var;
	PropVariantInit(&var);
	var.vt = VT_I8;
	var.hVal.QuadPart = 0;

	HRESULT hr = m_sourceReader->SetCurrentPosition(GUID_NULL, var);
	PropVariantClear(&var);

	if (FAILED(hr))
	{
		OutputDebugStringA("[MFVideoDecoder] SeekToStart failed\n");
		return;
	}

	m_isFinished = false;
	m_hasFrame = false;
	m_currentTime = 0;
	m_accumulator = 0.0;

	// 最初のフレームを読み込む
	ReadNextFrame();

	OutputDebugStringA("[MFVideoDecoder] SeekToStart succeeded\n");
}

void MFVideoDecoder::Shutdown()
{
	UnlockCurrentBuffer();
	m_currentSample.Reset();
	m_currentBuffer.Reset();
	m_sourceReader.Reset();
	m_hasFrame = false;
	m_isFinished = true;
}

void MFVideoDecoder::UnlockCurrentBuffer()
{
	if (m_lockedData && m_currentBuffer)
	{
		if (m_locked2D)
		{
			// Lock2Dでロックされた場合はUnlock2Dを使う
			Microsoft::WRL::ComPtr<IMF2DBuffer> buffer2D;
			HRESULT hr = m_currentBuffer.As(&buffer2D);
			if (SUCCEEDED(hr))
			{
				buffer2D->Unlock2D();
			}
		}
		else
		{
			m_currentBuffer->Unlock();
		}
		m_lockedData = nullptr;
		m_lockedStride = 0;
		m_locked2D = false;
	}
}

bool MFVideoDecoder::ReadNextFrame()
{
	if (!m_sourceReader)
		return false;

	DWORD streamIndex = 0;
	DWORD flags = 0;
	LONGLONG timestamp = 0;
	Microsoft::WRL::ComPtr<IMFSample> sample;

	HRESULT hr = m_sourceReader->ReadSample(
		static_cast<DWORD>(MF_SOURCE_READER_FIRST_VIDEO_STREAM),
		0,
		&streamIndex,
		&flags,
		&timestamp,
		&sample);

	if (FAILED(hr))
	{
		OutputDebugStringA("[MFVideoDecoder] ReadSample failed\n");
		return false;
	}

	if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
	{
		OutputDebugStringA("[MFVideoDecoder] End of stream\n");
		return false;
	}

	if (!sample)
	{
		// サンプルが取れなかったがストリームは終了していない場合
		return true;
	}

	// 新しいフレームを保持
	m_currentSample = sample;
	m_currentBuffer.Reset();

	hr = m_currentSample->ConvertToContiguousBuffer(&m_currentBuffer);
	if (FAILED(hr))
	{
		OutputDebugStringA("[MFVideoDecoder] ConvertToContiguousBuffer failed\n");
		m_currentSample.Reset();
		return false;
	}

	m_hasFrame = true;
	m_currentTime = timestamp;
	return true;
}
