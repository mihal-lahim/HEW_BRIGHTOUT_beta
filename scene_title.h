////////////////////////////////////
//タイトルシーン
//Author :hiroshi kasiwagi
//Date   :2025/12/20
////////////////////////////////////
#ifndef SCENE_TITLE_H
#define SCENE_TITLE_H
#include "scene.h"
class SceneTitle : public Scene
{
public:
	void Enter() override;
	void Update(double) override;
	void Draw() override;
	void Exit() override;
};

#endif // SCENE_TITLE_H
