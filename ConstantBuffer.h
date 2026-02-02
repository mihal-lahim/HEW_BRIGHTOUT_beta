#ifndef CONSTANT_BUFFER_H
#define CONSTANT_BUFFER_H

#include "Resource.h"
#include <wrl/client.h>
#include <d3d11.h>
#include <DirectXMath.h>

class ICBData
{
public:
	virtual ~ICBData() = default;
	virtual const void* Get() const = 0;
};

template <typename T>
class CBData : public ICBData
{
public:
	T data;
	virtual ~CBData() = default;
	virtual const void* Get() const override { return &data; }
};


class ConstantBuffer : public Resource
{
public:
	enum class UsageType
	{
		DEFAULT,
		DYNAMIC,
	};
	ConstantBuffer(UsageType type)
		: m_usageType(type)
	{
	}
	virtual ~ConstantBuffer() = default;
	virtual void CreateBuffers(GraphicsDevice& device) override = 0;
	virtual void UpdateBuffer(GraphicsDevice& device, const void* data) = 0;
	virtual ID3D11Buffer* GetBuffer() const { return m_constantBuffer.Get(); }
protected:
	UsageType m_usageType = UsageType::DEFAULT;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer = nullptr;


	void CreateBuffers(GraphicsDevice* device, UsageType type, int size);
	void UpdateBuffer(GraphicsDevice* device, UsageType type, const void* data, int size);
};

class PerFrameCB : public ConstantBuffer
{
public:
	struct CBData : public ICBData
	{
		DirectX::XMFLOAT4 AmbientColor = {};
		DirectX::XMFLOAT3 DirectionalVec = {};
		const float padding0 = 0.0f; // 16バイトアライメントのためのパディング
		DirectX::XMFLOAT4 DirectionalColor = {};
	};
	PerFrameCB()
		: ConstantBuffer(UsageType::DEFAULT)
	{
	}
	virtual void CreateBuffers(GraphicsDevice& device) override;
	virtual void UpdateBuffer(GraphicsDevice& device, const void* data) override;
	virtual ~PerFrameCB() = default;
};

class PerCameraCB : public ConstantBuffer
{
public:
	struct CBData : public ICBData
	{
		DirectX::XMFLOAT4X4 ViewMatrix = {};
		DirectX::XMFLOAT4X4 ProjectionMatrix = {};
		DirectX::XMFLOAT3 CameraPosition = {};
		const float padding0 = 0.0f; // 16バイトアライメントのためのパディング
	};
	PerCameraCB()
		: ConstantBuffer(UsageType::DYNAMIC)
	{
	}
	virtual void CreateBuffers(GraphicsDevice& device) override;
	virtual void UpdateBuffer(GraphicsDevice& device, const void* data) override;
	virtual ~PerCameraCB() = default;
};

class PerObjectCB : public ConstantBuffer
{
public:
	struct CBData : public ICBData
	{
		DirectX::XMFLOAT4X4 WorldMatrix = {};
	};
	PerObjectCB()
		: ConstantBuffer(UsageType::DYNAMIC)
	{
	}
	virtual void CreateBuffers(GraphicsDevice& device) override;
	virtual void UpdateBuffer(GraphicsDevice& device, const void* data) override;
	virtual ~PerObjectCB() = default;
};

#endif
