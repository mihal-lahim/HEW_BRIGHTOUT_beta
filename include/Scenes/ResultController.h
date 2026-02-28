#ifndef RESULT_CONTROLLER_H
#define RESULT_CONTROLLER_H

#include "ScriptComponent.h"

class FadeController;

class ResultController : public ScriptComponent
{
public:
	void Start() override;
	void Update() override;

private:
	FadeController* m_fadeController = nullptr;
	bool m_fadeReady = false;
	bool m_isTransitioning = false;
};

#endif
