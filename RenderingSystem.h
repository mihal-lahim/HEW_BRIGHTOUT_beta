
#ifndef RENDERING_SYSTEM_H
#define RENDERING_SYSTEM_H

#include "GraphicsDevice.h"

class EngineCore;
class Scene;

class RenderingSystem
{
public:
	RenderingSystem(EngineCore* engineCore)
		: m_engineCore(engineCore)
	{ 
		Initialize();
	}
	~RenderingSystem() 
	{ 
		Finalize();
	}

	// グラフィックスデバイス設定メソッド
	void SetGraphicsDevice(GraphicsDevice* graphicsDevice) 
	{ 
		m_graphicsDevice = graphicsDevice; 
	}

	// グラフィックスデバイス取得メソッド
	GraphicsDevice& GetGraphicsDevice() 
	{ 
		return *m_graphicsDevice;
	}

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
