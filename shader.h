/*==============================================================================

   シェーダー [shader.h]
														 Author : Rintarou Sumi
														 Date   : 2025/05/15
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef SHADER_H
#define	SHADER_H

#include <d3d11.h>
#include <DirectXMath.h>
#include "GraphicsDevice.h"
#include "Resource.h"
#include "ConstantBuffer.h"

bool Shader_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Shader_Finalize();

void Shader_SetProjectionMatrix(const DirectX::XMMATRIX& matrix);
void Shader_SetWorldMatrix(const DirectX::XMMATRIX& matrix);
void Shader_SetColor(const DirectX::XMFLOAT4& color);

void Shader_Begin();

struct ICB
{
};

class Shader : public Resource
{
public:
	virtual ~Shader() = default;
	virtual void CreateBuffers(GraphicsDevice& device) override = 0;
	virtual void Bind(GraphicsDevice& device) = 0;

	void UpdatePerFrameCB(GraphicsDevice& device, PerFrameCB::CBData& data)
	{
		m_perFrameCB->UpdateBuffer(device, &data);
	}
	void UpdatePerCameraCB(GraphicsDevice& device, PerCameraCB::CBData& data)
	{
		m_perCameraCB->UpdateBuffer(device, &data);
	}
	void UpdatePerObjectCB(GraphicsDevice& device, PerObjectCB::CBData& data)
	{
		m_perObjectCB->UpdateBuffer(device, &data);
	}

	virtual ICBData* CreateMaterialCB() = 0;
	virtual void UpdateMaterialCB(GraphicsDevice& device, const ICBData* data) = 0;
protected:
	PerFrameCB* m_perFrameCB = nullptr;
	PerCameraCB* m_perCameraCB = nullptr;
	PerObjectCB* m_perObjectCB = nullptr;
};


class VertexShader : public Shader
{
public:
	virtual ~VertexShader() = default;
	ICBData* CreateMaterialCB() override
	{
		return nullptr;
	}

	void UpdateMaterialCB(GraphicsDevice&, const ICBData*) override
	{
	}
protected:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
};

class PixelShader : public Shader
{
public:
	virtual ~PixelShader() = default;
protected:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
};


class VertexShader2D : public VertexShader
{
public:
	virtual ~VertexShader2D() = default;
};

class PixelShader2D : public PixelShader
{
public:
	virtual ~PixelShader2D() = default;
};

class VertexShader3D : public VertexShader
{
public:
	virtual ~VertexShader3D() = default;
};

class PixelShader3D : public PixelShader
{
public:
	virtual ~PixelShader3D() = default;
};


class SpriteVS : public VertexShader2D
{
public:
	virtual void CreateBuffers(GraphicsDevice& device) override;
	virtual void Bind(GraphicsDevice& device) override;
};

class SpritePS : public PixelShader2D
{
public:
	struct PerMaterialCB : public ICB
	{
		DirectX::XMFLOAT4 Color;
	};

	ICBData* CreateMaterialCB() override
	{
		return new CBData<PerMaterialCB>;
	}

	void UpdateMaterialCB(GraphicsDevice& device, const ICBData* data) override
	{
		m_materialCB->UpdateBuffer(device, data->Get());
	}

	virtual void CreateBuffers(GraphicsDevice& device) override;
	virtual void Bind(GraphicsDevice& device) override;

private:
	ConstantBuffer* m_materialCB = nullptr;
};

class MeshVS : public VertexShader3D
{
public:
	virtual void CreateBuffers(GraphicsDevice& device) override;
	virtual void Bind(GraphicsDevice& device) override;
};

class MeshPS : public PixelShader3D
{
public:
	struct PerMaterialCB : public ICB
	{
		DirectX::XMFLOAT4 Color;
	};

	ICBData* CreateMaterialCB() override
	{
		return new CBData<PerMaterialCB>;
	}

	void UpdateMaterialCB(GraphicsDevice& device, const ICBData* data) override
	{
		m_materialCB->UpdateBuffer(device, data->Get());
	}
	virtual void CreateBuffers(GraphicsDevice& device) override;
	virtual void Bind(GraphicsDevice& device) override;
private:
	ConstantBuffer* m_materialCB = nullptr;
};


#endif
