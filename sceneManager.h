////////////////////////////////////
//シーン管理マネージャー
//Author :hiroshi kasiwagi
//Date   :2025/12/20
////////////////////////////////////
#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H
#include "scene.h"
class SceneManager
{
private:
	Scene* m_scene = nullptr;
public:
	void Enter();
	void Update(double);
	void Draw();
	void Exit();

	void ChangeScene(Scene& newScene);
};
#endif // SCENE_MANAGER_H
