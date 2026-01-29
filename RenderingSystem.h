
#ifndef RENDERING_SYSTEM_H
#define RENDERING_SYSTEM_H

#include <vector>
#include <map>

class Camera;
class MeshRenderer;

class RenderingSystem
{
private:
	// 登録されているカメラのマップ（優先度順）
	std::map<int, std::vector<Camera*>> m_Cameras;

	// 登録されているメッシュレンダラーの配列
	std::vector<MeshRenderer*> m_MeshRenderers;
public:
	RenderingSystem() { Initialize(); }
	~RenderingSystem() { Finalize(); }

	void Initialize();
	void Finalize();

	// 描画メソッド
	void Render();

	// カメラ・メッシュレンダラー登録メソッド
	void RegisterCamera(Camera* camera);
	void RegisterMeshRenderer(MeshRenderer* meshRenderer);

	// カメラ・メッシュレンダラー登録解除メソッド
	void UnregisterCamera(Camera* camera);
	void UnregisterMeshRenderer(MeshRenderer* meshRenderer);
};


#endif
