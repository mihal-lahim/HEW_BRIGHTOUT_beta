
#ifndef RENDERING_SYSTEM_H
#define RENDERING_SYSTEM_H

#include "GraphicsDevice.h"
#include "Mesh.h"
#include <memory>

class Camera;
class Scene;
class MeshRenderer;
class EngineCore;

class RenderingSystem
{
public:
	RenderingSystem(EngineCore* engineCore)
		: m_engineCore(engineCore)
	{ Initialize(); }
	~RenderingSystem() { Finalize(); }

	// グラフィックスデバイス設定メソッド
	void SetDevice(GraphicsDevice* device);

	void Initialize();
	void Finalize();

	// 描画メソッド
	void Render(const Scene& scene);


	// プリミティブメッシュ（今はとりあえずRenderingSystemに持たせておく）
	static CubeMesh cubeMesh;
	static PlaneMesh planeMesh;
	static SphereMesh sphereMesh;
	static CapsuleMesh capsuleMesh;
	static CylinderMesh cylinderMesh;
private:
	// 所属するエンジンコア
	EngineCore* m_engineCore = nullptr;

	// グラフィックスデバイス
	GraphicsDevice* m_graphicsDevice = nullptr;
};


#endif
