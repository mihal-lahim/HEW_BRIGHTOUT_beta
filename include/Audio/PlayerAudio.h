#pragma once
#include "ScriptComponent.h"
#include "EventSystem.h"

class PlayerAudio : public ScriptComponent
{
public:
    void Awake() override;
    void OnDestroy() override;

private:
    int m_FootstepSE = -1;
    bool m_IsMoving = false;

    EventSystem::HandleID m_MoveStartHandle = 0;
    EventSystem::HandleID m_MoveStopHandle = 0;
};