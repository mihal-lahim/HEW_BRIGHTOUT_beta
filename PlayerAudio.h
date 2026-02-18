#pragma once
#include "ScriptComponent.h"

class PlayerAudio : public ScriptComponent
{
public:
    void Awake() override;
   // void Update() override;

private:
    int m_FootstepSE = -1;
    bool m_IsMoving = false;
};