#ifndef ENEMY_H
#define ENEMY_H

#include "Movement.h"
#include "audio.h"

class Enemy : public Movement
{
public:
	float MoveSpeed = 3.0f;

	void Start() override
	{
		Movement::Start();
		m_target = GetGameObjectByTag("Player");

		// Ž€–SSE“Ç‚Ýž‚Ý
		m_DeathSE = LoadAudio("sound/enemy_die.wav"); 
		if (m_DeathSE >= 0)
		{
			SetAudioVolume(m_DeathSE, 0.7f);
		}

		// i•K—v‚È‚ç‘«‰¹‚à‚±‚±‚Å“Ç‚Ýž‚Þj
		// m_MoveSE = LoadAudio("sound/enemy_step.wav");
		if (m_MoveSE >= 0)
		{
			SetAudioVolume(m_MoveSE, 0.4f);
		}
	}

	void Update() override
	{
		if (!m_target)
		{
			m_target = GetGameObjectByTag("Player");
			if (!m_target)
			{
				return;
			}
		}

		Vector3 direction = m_target->transform().position() - gameObject().transform().position();
		direction.y = 0.0f;

		float length = direction.Length();
		if (length <= 0.1f)
		{
			MoveVec = { 0.0f, 0.0f, 0.0f };
			// ’âŽ~‚µ‚½‚çSE‚ðŽ~‚ß‚é
			if (m_isMoving && m_MoveSE >= 0)
			{
				StopAudio(m_MoveSE);
				m_isMoving = false;
			}
			return;
		}

		// ˆÚ“®ŠJŽnŽž‚ÉSE‚ðƒ‹[ƒv‚ÅÄ¶
		if (!m_isMoving && m_MoveSE >= 0)
		{
			PlayAudio(m_MoveSE, true);
			m_isMoving = true;
		}

		MoveVec = direction.Normalize() * MoveSpeed;
		RotateByMoveVec();
	}

	// “|‚³‚ê‚½/DestroyŽž‚ÉŒÄ‚Î‚ê‚é
	void OnDestroy() override
	{
		// ˆÚ“®’†‚Ìƒ‹[ƒvSE‚ðŽ~‚ß‚é
		if (m_isMoving && m_MoveSE >= 0)
		{
			StopAudio(m_MoveSE);
			m_isMoving = false;
		}

		// Ž€–SSE‚ðƒƒ“ƒVƒ‡ƒbƒg‚ÅÄ¶
		if (m_DeathSE >= 0)
		{
			PlayAudioOneShot(m_DeathSE, 0.9f);
		}

		Movement::OnDestroy();
	}

private:
	GameObject* m_target = nullptr;
	int m_MoveSE = -1;
	bool m_isMoving = false;

	// “|‚³‚ê‚½‚Æ‚«‚ÌSE
	int m_DeathSE = -1;
};

#endif
