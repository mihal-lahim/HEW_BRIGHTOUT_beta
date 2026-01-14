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
	void Enter(Scene&);
	void Update(Scene&);
	void Draw(Scene&);
	void Exit(Scene&);

	void ChangeScene(Scene&);
};
#endif // SCENE_MANAGER_H
