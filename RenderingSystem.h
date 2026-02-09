
#ifndef RENDERING_SYSTEM_H
#define RENDERING_SYSTEM_H

#include "GraphicsDevice.h"
#include "ConstantBuffer.h"
#include <DirectXMath.h>

class EngineCore;
class Scene;
class Texture;
class MeshRenderer;
class SkinnedMeshRenderer;
class Transform;
class ShaderProgram;
enum class RenderQueue;
class Texture;

class RenderingSystem
{
public:
	RenderingSystem(EngineCore* engineCore)
		: m_engineCore(engineCore)
	{ 
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

	void Initialize(GraphicsDevice& graphicsDevice);
	void Finalize();

	// 描画メソッド
	void Render(const Scene& scene);

private:
	struct PerFrameConstants
	{
		DirectX::XMFLOAT4 ambient_light_color = {};
		DirectX::XMFLOAT4 directional_light_color = {};
		DirectX::XMFLOAT4 directional_light_vector = {};
	};

	struct PerCameraConstants
	{
		DirectX::XMFLOAT4X4 view = {};
		DirectX::XMFLOAT4X4 projection = {};
	};

	struct PerObjectConstants
	{
		DirectX::XMFLOAT4X4 world = {};
	};

	ConstantBuffer m_perFrameBuffer = {};
	ConstantBuffer m_perCameraBuffer = {};
	ConstantBuffer m_perObjectBuffer = {};
	Texture* m_defaultTexture = nullptr;
	ShaderProgram* m_defaultMeshShader = nullptr;
	ShaderProgram* m_defaultSkinnedShader = nullptr;
	bool m_buffersInitialized = false;

	void UpdatePerFrame();
	void UpdatePerCamera(const Scene& scene);
	void UpdatePerObject(const Transform& transform);
	void RenderMeshRenderer(MeshRenderer& renderer);
	void RenderSkinnedMeshRenderer(SkinnedMeshRenderer& renderer);
	void ApplyRenderQueue(RenderQueue queue);

	// 所属するエンジンコア
	EngineCore* m_engineCore = nullptr;

	// グラフィックスデバイス
	GraphicsDevice* m_graphicsDevice = nullptr;
};


#endif
