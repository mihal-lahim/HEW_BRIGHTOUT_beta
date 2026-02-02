#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H

#include "Component.h"
#include "Material.h"
#include "Shader.h"
#include <wrl/client.h>

class Texture;

class SpriteRenderer : public Component
{
public:
	SpriteRenderer() = default;
	virtual ~SpriteRenderer() = default;

	void SetTexture(Texture* texture)
	{
		m_texture = texture;
	}

	void SetVertexShader(VertexShader2D* vertexShader)
	{
		m_vertexShader = vertexShader;
	}

	VertexShader2D* GetVertexShader() const
	{
		return m_vertexShader;
	}

	void SetMaterialInstance(MaterialInstance2D* materialInstance)
	{
		m_materialInstance = materialInstance;
	}

	MaterialInstance2D* GetMaterialInstance() const
	{
		return m_materialInstance;
	}

	void Render(GraphicsDevice& device);

private:
	struct Vertex
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

	void EnsureVertexBuffer(GraphicsDevice& device);

	Texture* m_texture = nullptr;
	VertexShader2D* m_vertexShader = nullptr;
	MaterialInstance2D* m_materialInstance = nullptr;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer = nullptr;
};



#endif
