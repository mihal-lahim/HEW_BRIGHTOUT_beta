#pragma once



void InitAudio();
void UninitAudio();


int LoadAudio(const char* FileName);
void UnloadAudio(int Index);
void PlayAudio(int Index, bool Loop = false);
void StopAudio(int Index);
void SetAudioVolume(int Index, float volume);

void PlayAudioOneShot(int Index, float volume = 1.0f);

