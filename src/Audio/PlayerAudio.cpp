#include "PlayerAudio.h"
#include "EventSystem.h"
#include "audio.h"

void PlayerAudio::Awake()
{
    m_FootstepSE = LoadAudio("sound/hito_ge_run_hai_ss01.wav");

    EventSystem::Register("Player_MoveStart", [this]()
        {
            if (!m_IsMoving)
            {
                PlayAudio(m_FootstepSE, true);
                SetAudioVolume(m_FootstepSE, 0.3f);
                m_IsMoving = true;
            }
        });

    EventSystem::Register("Player_MoveStop", [this]()
        {
            if (m_IsMoving)
            {
                StopAudio(m_FootstepSE);
                m_IsMoving = false;
            }
        });
}