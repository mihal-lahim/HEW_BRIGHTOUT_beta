
#ifndef RENDERING_SYSTEM_H
#define RENDERING_SYSTEM_H

#include "GraphicsDevice.h"


class Camera;
class Scene;
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
	GraphicsDevice& GetGraphicsDevice() { return *m_graphicsDevice; }

	void Initialize();
	void Finalize();

	// 描画メソッド
	void Render(const Scene& scene);

private:
	// 所属するエンジンコア
	EngineCore* m_engineCore = nullptr;

	// グラフィックスデバイス
	GraphicsDevice* m_graphicsDevice = nullptr;
};


#endif
