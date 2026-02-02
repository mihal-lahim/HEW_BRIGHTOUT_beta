
#ifndef RENDERING_SYSTEM_H
#define RENDERING_SYSTEM_H

#include "GraphicsDevice.h"
#include "Material.h"
#include <memory>


class Camera;
class Scene;
class EngineCore;
class PixelShader3D;
class VertexShader3D;
class PixelShader2D;
class VertexShader2D;

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

	VertexShader3D* m_defaultVertexShader = nullptr;
	PixelShader3D* m_defaultPixelShader = nullptr;
	std::unique_ptr<MaterialInstance3D> m_defaultMaterialInstance;
	VertexShader2D* m_defaultSpriteVertexShader = nullptr;
	PixelShader2D* m_defaultSpritePixelShader = nullptr;
	std::unique_ptr<MaterialInstance2D> m_defaultSpriteMaterialInstance;
};


#endif
