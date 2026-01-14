////////////////////////////////////
//タイトルシーン
//Author :hiroshi kasiwagi
//Date   :2025/12/20
////////////////////////////////////
#include "scene_title.h"
#include "ObjectManager.h"



void SceneTitle::Enter()
{
	m_object_manager->Initialize();
}

void SceneTitle::Update(double elapsed_time)
{
	m_object_manager->Cycle(elapsed_time);
}

void SceneTitle::Draw()
{
}

void SceneTitle::Exit()
{
	m_object_manager->Finalize();
}
