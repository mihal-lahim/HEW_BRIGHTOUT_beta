////////////////////////////////////
//シーン管理マネージャー
//Author :hiroshi kasiwagi
//Date   :2025/12/20
////////////////////////////////////
#include "sceneManager.h"

void SceneManager::Enter()
{
	// シーンのEnter処理
	if (m_scene) {
		m_scene->Enter();
	}
}
void SceneManager::Update(double elapsed_time)
{
	// シーンのUpdate処理
	if (m_scene) {
		m_scene->Update(elapsed_time);
	}
}
void SceneManager::Draw()
{
	// シーンのDraw処理
	if (m_scene) {
		m_scene->Draw();
	}
}
void SceneManager::Exit()
{
	// シーンのExit処理
	if (m_scene) {
		m_scene->Exit();
	}
}
void SceneManager::ChangeScene(Scene& newScene)
{
	// 同じシーンなら何もしない
	if (m_scene == &newScene)
		return;

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

