#ifndef RENDERING_SYSTEM_H
#define RENDERING_SYSTEM_H

#include "GraphicsDevice.h"
#include "ConstantBuffer.h"
#include <DirectXMath.h>
#include <memory>
#include "Mesh.h"

class EngineCore;
class Scene;
class Texture;
class MeshRenderer;
class Transform;
enum class RenderQueue;
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

    void SetGraphicsDevice(GraphicsDevice* graphicsDevice)
    {
        m_graphicsDevice = graphicsDevice;
    }

    GraphicsDevice& GetGraphicsDevice()
    {
        return *m_graphicsDevice;
    }

    void Initialize(GraphicsDevice& graphicsDevice);
    void Finalize();
    void Render(const Scene& scene);

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

    // Åö UiVS ÇÃ register(b4) Ç∆àÍív
    struct UIParamsConstants
    {
        float ScreenWidth;
        float ScreenHeight;
        float UseScreenSpace;
        float pad;
    };

    std::shared_ptr<Mesh> CreateUIQuad();
    std::shared_ptr<Mesh> CreateUIQuadWithUV(float u0, float u1);

private:

    ConstantBuffer m_perFrameBuffer = {};
    ConstantBuffer m_perCameraBuffer = {};
    ConstantBuffer m_perObjectBuffer = {};
    ConstantBuffer m_uiParamsBuffer = {};   // Åö í«â¡

    void MaterialLoadingProcess(Material& material);
    void UpdatePerFrame();
    void UpdatePerCamera(const Camera& camera);
    void ApplyRenderQueue(RenderQueue queue);

    EngineCore* m_engineCore = nullptr;
    GraphicsDevice* m_graphicsDevice = nullptr;

    std::shared_ptr<Mesh> m_uiQuad;
};

#endif