#ifndef TITLE_CONTROLLER_H
#define TITLE_CONTROLLER_H

#include "ScriptComponent.h"

class FadeController;

class TitleController : public ScriptComponent
{
public:
	void Start() override;
	void Update() override;

private:
	FadeController* m_fadeController = nullptr;
	bool m_isTransitioning = false;
	bool m_fadeReady = false;  // フェードイン完了後に入力受付
};

#endif
