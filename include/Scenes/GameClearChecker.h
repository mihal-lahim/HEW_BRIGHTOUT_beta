#ifndef GAME_CLEAR_CHECKER_H
#define GAME_CLEAR_CHECKER_H

#include "ScriptComponent.h"
#include <vector>

class FadeController;
class PowerPlant;

class GameClearChecker : public ScriptComponent
{
public:
	void Start() override;
	void Update() override;

private:
	FadeController* m_fadeController = nullptr;
	bool m_isTransitioning = false;
	std::vector<PowerPlant*> m_cachedPlants;
	bool m_plantsCached = false;
};

#endif
