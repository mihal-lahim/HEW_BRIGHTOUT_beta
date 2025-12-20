////////////////////////////////////
//シーン管理マネージャー
//Author :hiroshi kasiwagi
//Date   :2025/12/20
////////////////////////////////////
#include "sceneManager.h"

void SceneManager::Enter(Scene&)
{
	// シーンのEnter処理
	if (m_scene) {
		m_scene->Enter();
	}
}
void SceneManager::Update(Scene&)
{
	// シーンのUpdate処理
	if (m_scene) {
		m_scene->Update();
	}
}
void SceneManager::Draw(Scene&)
{
	// シーンのDraw処理
	if (m_scene) {
		m_scene->Draw();
	}
}
void SceneManager::Exit(Scene&)
{
	// シーンのExit処理
	if (m_scene) {
		m_scene->Exit();
	}
}
void SceneManager::ChangeScene(Scene& newScene)
{
	// 現在のシーンのExit処理
	if (m_scene) {
		m_scene->Exit();
	}
	// 新しいシーンに変更
	m_scene = &newScene;
	// 新しいシーンのEnter処理
	if (m_scene) {
		m_scene->Enter();
	}
}

