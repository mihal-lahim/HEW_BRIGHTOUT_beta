//////////////////////
//  オーディオ処理	 
// Author: Namisyo
//////////////////////

#include <xaudio2.h>
#include <assert.h>
#include <string>
#include "audio.h"

#pragma comment(lib, "winmm.lib")

static IXAudio2* g_Xaudio{};
static IXAudio2MasteringVoice* g_MasteringVoice{};

struct AUDIO
{
	IXAudio2SourceVoice* SourceVoice{};
	BYTE* SoundData{};

	int						Length{};
	int						PlayLength{};

	// 追加: フォーマット情報を保持しておく（ワンショット用に必要）
	WAVEFORMATEX wfx{};
};

#define AUDIO_MAX 100
static AUDIO g_Audio[AUDIO_MAX]{};


void InitAudio()
{
	// 既存のオーディオリソースをクリーンアップ
	if (g_Xaudio)
	{
		for (int i = 0; i < AUDIO_MAX; i++)
		{
			if (g_Audio[i].SourceVoice)
			{
				g_Audio[i].SourceVoice->Stop();
				g_Audio[i].SourceVoice->DestroyVoice();
				g_Audio[i].SourceVoice = nullptr;
			}
			delete[] g_Audio[i].SoundData;
			g_Audio[i].SoundData = nullptr;
			g_Audio[i].Length = 0;
			g_Audio[i].PlayLength = 0;
		}
		if (g_MasteringVoice)
		{
			g_MasteringVoice->DestroyVoice();
			g_MasteringVoice = nullptr;
		}
		g_Xaudio->Release();
		g_Xaudio = nullptr;
	}

	// XAudio生成
	XAudio2Create(&g_Xaudio, 0);

	// マスタリングボイス生成
	g_Xaudio->CreateMasteringVoice(&g_MasteringVoice);
}

void UninitAudio()
{
	for (int i = 0; i < AUDIO_MAX; i++)
	{
		if (g_Audio[i].SourceVoice)
		{
			g_Audio[i].SourceVoice->Stop();
			g_Audio[i].SourceVoice->DestroyVoice();
			g_Audio[i].SourceVoice = nullptr;
		}
		delete[] g_Audio[i].SoundData;
		g_Audio[i].SoundData = nullptr;
	}
	if (g_MasteringVoice)
	{
		g_MasteringVoice->DestroyVoice();
		g_MasteringVoice = nullptr;
	}
	if (g_Xaudio)
	{
		g_Xaudio->Release();
		g_Xaudio = nullptr;
	}
}


int LoadAudio(const char* FileName)
{
	assert(g_Xaudio);
	int index = -1;
	std::string resolvedPath = (FileName != nullptr) ? FileName : "";
	if (resolvedPath.rfind("sound/", 0) == 0)
	{
		resolvedPath = "resource/" + resolvedPath;
	}

	for (int i = 0; i < AUDIO_MAX; i++)
	{
		if (g_Audio[i].SourceVoice == nullptr)
		{
			index = i;
			break;
		}
	}

	if (index == -1)
		return -1;

	// サウンドデータ読込
	WAVEFORMATEX wfx = { 0 };

	{
		HMMIO hmmio = NULL;
		MMIOINFO mmioinfo = { 0 };
		MMCKINFO riffchunkinfo = { 0 };
		MMCKINFO datachunkinfo = { 0 };
		MMCKINFO mmckinfo = { 0 };
		UINT32 buflen;
		LONG readlen;


		hmmio = mmioOpen((LPSTR)resolvedPath.c_str(), &mmioinfo, MMIO_READ);
		assert(hmmio);

		riffchunkinfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
		mmioDescend(hmmio, &riffchunkinfo, NULL, MMIO_FINDRIFF);

		mmckinfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
		mmioDescend(hmmio, &mmckinfo, &riffchunkinfo, MMIO_FINDCHUNK);

		if (mmckinfo.cksize >= sizeof(WAVEFORMATEX))
		{
			mmioRead(hmmio, (HPSTR)&wfx, sizeof(wfx));
		}
		else
		{
			PCMWAVEFORMAT pcmwf = { 0 };
			mmioRead(hmmio, (HPSTR)&pcmwf, sizeof(pcmwf));
			memset(&wfx, 0x00, sizeof(wfx));
			memcpy(&wfx, &pcmwf, sizeof(pcmwf));
			wfx.cbSize = 0;
		}
		mmioAscend(hmmio, &mmckinfo, 0);

		datachunkinfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
		mmioDescend(hmmio, &datachunkinfo, &riffchunkinfo, MMIO_FINDCHUNK);



		buflen = datachunkinfo.cksize;
		g_Audio[index].SoundData = new unsigned char[buflen];
		readlen = mmioRead(hmmio, (HPSTR)g_Audio[index].SoundData, buflen);


		g_Audio[index].Length = readlen;
		g_Audio[index].PlayLength = readlen / wfx.nBlockAlign;


		mmioClose(hmmio, 0);
	}

	// フォーマット情報を保存
	g_Audio[index].wfx = wfx;

	// ソースボイス生成（既存の単一再生用に）
	g_Xaudio->CreateSourceVoice(&g_Audio[index].SourceVoice, &g_Audio[index].wfx);
	assert(g_Audio[index].SourceVoice);


	return index;
}

void UnloadAudio(int Index)
{


	if (Index < 0 || Index >= AUDIO_MAX)
		return;


	if (g_Audio[Index].SourceVoice)
	{
		g_Audio[Index].SourceVoice->Stop();
		g_Audio[Index].SourceVoice->DestroyVoice();

		g_Audio[Index].SourceVoice = nullptr; 

		

	}

	delete[] g_Audio[Index].SoundData;
	g_Audio[Index].SoundData = nullptr;

	g_Audio[Index].Length = 0;
	g_Audio[Index].PlayLength = 0;
}

void PlayAudio(int Index, bool Loop)
{
	if (Index < 0 || Index >= AUDIO_MAX || !g_Audio[Index].SourceVoice)
		return;

	g_Audio[Index].SourceVoice->Stop();
	g_Audio[Index].SourceVoice->FlushSourceBuffers();


	// バッファ設定
	XAUDIO2_BUFFER bufinfo;

	memset(&bufinfo, 0x00, sizeof(bufinfo));
	bufinfo.AudioBytes = g_Audio[Index].Length;
	bufinfo.pAudioData = g_Audio[Index].SoundData;
	bufinfo.PlayBegin = 0;
	bufinfo.PlayLength = g_Audio[Index].PlayLength;

	// ループ設定
	if (Loop)
	{
		bufinfo.LoopBegin = 0;
		bufinfo.LoopLength = g_Audio[Index].PlayLength;
		bufinfo.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

	g_Audio[Index].SourceVoice->SubmitSourceBuffer(&bufinfo, NULL);


	// 再生
	g_Audio[Index].SourceVoice->Start();

}

void StopAudio(int Index)
{
	if (Index < 0 || Index >= AUDIO_MAX)
		return;

	if (g_Audio[Index].SourceVoice)
	{
		g_Audio[Index].SourceVoice->Stop();
		g_Audio[Index].SourceVoice->FlushSourceBuffers();
	}
}

void SetAudioVolume(int Index, float volume)
{
	if (Index < 0 || Index >= AUDIO_MAX)
		return;

	if (g_Audio[Index].SourceVoice)
	{
		g_Audio[Index].SourceVoice->SetVolume(volume);
	}
}

// コールバッククラス（再生終了時に自身で SourceVoice を破棄して delete する）
class OneShotVoiceCallback : public IXAudio2VoiceCallback
{
public:
	OneShotVoiceCallback() = default;
	virtual ~OneShotVoiceCallback() = default;

	// IXAudio2VoiceCallback の仮想関数を空実装
	STDMETHOD_(void, OnVoiceProcessingPassStart)(UINT32) override {}
	STDMETHOD_(void, OnVoiceProcessingPassEnd)() override {}
	STDMETHOD_(void, OnStreamEnd)() override {}
	STDMETHOD_(void, OnBufferStart)(void* pBufferContext) override { (void)pBufferContext; }
	STDMETHOD_(void, OnBufferEnd)(void* pBufferContext) override
	{
		auto* voice = static_cast<IXAudio2SourceVoice*>(pBufferContext);
		if (voice)
		{
			voice->Stop();
			voice->FlushSourceBuffers();
			voice->DestroyVoice();
		}
	}
	STDMETHOD_(void, OnLoopEnd)(void* pBufferContext) override { (void)pBufferContext; }
	STDMETHOD_(void, OnVoiceError)(void* pBufferContext, HRESULT error) override { (void)pBufferContext; (void)error; }
};

static OneShotVoiceCallback g_OneShotVoiceCallback;


void PlayAudioOneShot(int Index, float volume)
{
	if (Index < 0 || Index >= AUDIO_MAX)
		return;

	if (!g_Xaudio)
		return;

	// 必要なフォーマット/データは g_Audio[Index] に保持している
	if (g_Audio[Index].SoundData == nullptr)
		return;

	IXAudio2SourceVoice* oneVoice = nullptr;
	HRESULT hr = g_Xaudio->CreateSourceVoice(&oneVoice, &g_Audio[Index].wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, &g_OneShotVoiceCallback);
	if (FAILED(hr) || oneVoice == nullptr) {
		return;
	}

	// ボリューム設定
	oneVoice->SetVolume(volume);

	XAUDIO2_BUFFER bufinfo;
	memset(&bufinfo, 0x00, sizeof(bufinfo));
	bufinfo.AudioBytes = g_Audio[Index].Length;
	bufinfo.pAudioData = g_Audio[Index].SoundData;
	bufinfo.PlayBegin = 0;
	bufinfo.PlayLength = g_Audio[Index].PlayLength;
	bufinfo.Flags = XAUDIO2_END_OF_STREAM;
	bufinfo.pContext = oneVoice;

	// ワンショットなのでループは設定しない
	oneVoice->SubmitSourceBuffer(&bufinfo, NULL);
	oneVoice->Start();
}

