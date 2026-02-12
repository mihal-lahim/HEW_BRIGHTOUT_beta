
#ifndef RENDERING_SYSTEM_H
#define RENDERING_SYSTEM_H

#include "GraphicsDevice.h"
#include "ConstantBuffer.h"
#include <DirectXMath.h>

class EngineCore;
class Scene;
class Texture;
class MeshRenderer;
class Transform;
enum class RenderQueue;
class Texture;
class ShaderProgram;
class Material;
class Camera;

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


	// フレーム毎更新用定数バッファ構造体
	struct PerFrameConstants
	{
		DirectX::XMFLOAT4 ambient_light_color = {};
		DirectX::XMFLOAT4 directional_light_color = {};
		DirectX::XMFLOAT4 directional_light_vector = {};
	};

	// カメラ毎更新用定数バッファ構造体
	struct PerCameraConstants
	{
		DirectX::XMFLOAT4X4 view = {};
		DirectX::XMFLOAT4X4 projection = {};
	};

	// オブジェクト毎更新用定数バッファ構造体
	struct PerObjectConstants
	{
		DirectX::XMFLOAT4X4 world = {};
	};

private:

	// 各種定数バッファ
	ConstantBuffer m_perFrameBuffer = {};
	ConstantBuffer m_perCameraBuffer = {};
	ConstantBuffer m_perObjectBuffer = {};

	// マテリアルの遅延ロード処理
	void MaterialLoadingProcess(Material& material);

	void UpdatePerFrame();
	void UpdatePerCamera(const Camera& camera);
	void ApplyRenderQueue(RenderQueue queue);

	// 所属するエンジンコア
	EngineCore* m_engineCore = nullptr;

	// グラフィックスデバイス
	GraphicsDevice* m_graphicsDevice = nullptr;
};


#endif
