/*==============================================================================

   シーン管理[scene.h]
														 Author : syota
														 Date   : 2025/12/09
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef SCENE_H
#define SCENE_H
#include "ObjectManager.h"

class Scene
{
private:
	ObjectManager* object_manager;

public:
	virtual void Enter() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Exit() = 0;
	virtual ~Scene() = default;
};

#endif // SCENE_H


//enum Scene
//{
//    SCENE_TITLE,
//    SCENE_TUTORIAL,
//    SCENE_GAME,
//    //SCENE_FIRST_RESULT,    // 第1ラウンドの小リザルト
//    //SCENE_ROUND2,
//   // SCENE_SECOND_RESULT,    // 第2ラウンドの小リザルト
//   // SCENE_ROUND3,
//    SCENE_RESULT, // 最終リザルト
//    SCENE_NAME_INPUT,
//    SCENE_MAX
//};
//
//void Scene_SetNextScene(Scene scene);
//void Scene_ChangeScene();

