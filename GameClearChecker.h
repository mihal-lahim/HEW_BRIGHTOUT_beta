#ifndef GAME_CLEAR_CHECKER_H
#define GAME_CLEAR_CHECKER_H

#include "ScriptComponent.h"

class FadeController;

class GameClearChecker : public ScriptComponent
{
public:
	void Start() override;
	void Update() override;

private:
	FadeController* m_fadeController = nullptr;
	bool m_isTransitioning = false;
};

#endif
