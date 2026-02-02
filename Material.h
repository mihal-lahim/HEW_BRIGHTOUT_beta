#ifndef MATERIAL_H
#define MATERIAL_H

#include "Shader.h"
#include <memory>


class MaterialInstance2D
{
public:
	PixelShader2D* PixelShader = nullptr;

	std::unique_ptr<ICBData> MaterialCBData;

	void SetColor(const DirectX::XMFLOAT4& color)
	{
		auto* data = static_cast<CBData<SpritePS::PerMaterialCB>*>(MaterialCBData.get());
		data->data.Color = color;
	}

	void Apply(GraphicsDevice& device)
	{
		PixelShader->UpdateMaterialCB(device, MaterialCBData.get());
		PixelShader->Bind(device);
	}

};


class MaterialInstance3D
{
public:
	PixelShader3D* PixelShader = nullptr;

	std::unique_ptr<ICBData> MaterialCBData;

	void SetColor(const DirectX::XMFLOAT4& color)
	{
		if (!MaterialCBData)
		{
			return;
		}
		auto* data = static_cast<CBData<MeshPS::PerMaterialCB>*>(MaterialCBData.get());
		data->data.Color = color;
	}

	void Apply(GraphicsDevice& device)
	{
		if (!PixelShader)
		{
			return;
		}
		if (MaterialCBData)
		{
			PixelShader->UpdateMaterialCB(device, MaterialCBData.get());
		}
		PixelShader->Bind(device);
	}
};


class Material2D
{
public:
	PixelShader2D* PixelShader = nullptr;

	DirectX::XMFLOAT4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };

	virtual MaterialInstance2D* Instantiate() const
	{
		auto* instance = new MaterialInstance2D();
		instance->PixelShader = PixelShader;

		instance->MaterialCBData.reset((PixelShader->CreateMaterialCB()));

		instance->SetColor(Color);

		return instance;
	}
};

class Material3D
{
public:
	PixelShader3D* PixelShader = nullptr;

	DirectX::XMFLOAT4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };

	virtual MaterialInstance3D* Instantiate() const
	{
		auto* instance = new MaterialInstance3D();
		instance->PixelShader = PixelShader;
		instance->MaterialCBData.reset(PixelShader->CreateMaterialCB());

		instance->SetColor(Color);

		return instance;
	}
};


#endif
