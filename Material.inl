#ifndef MATERIAL_INL
#define MATERIAL_INL

#include "ResourceSystem.h"


template<typename VS, typename PS>
	requires std::is_base_of_v<VertexShader, VS>&& std::is_base_of_v<PixelShader, PS>
inline void ShaderT<VS, PS>::Bind(GraphicsDevice& device)
{
	if (vs)
	{
		vs->Bind(device);
	}
	if (ps)
	{
		ps->Bind(device);
	}
}


template<typename VS, typename PS>
	requires std::is_base_of_v<VertexShader, VS>&& std::is_base_of_v<PixelShader, PS>
inline bool MaterialImpl<VS, PS>::CreateBuffer(GraphicsDevice& device)
{
	cb = m_resourceSystem->template Load<ConstantBuffer<CBType>>();

	shader.vs = m_resourceSystem->template Load<VS>();
	shader.ps = m_resourceSystem->template Load<PS>();

	return true;
}

template<typename VS, typename PS>
	requires std::is_base_of_v<VertexShader, VS>&& std::is_base_of_v<PixelShader, PS>
inline void MaterialImpl<VS, PS>::UpdateBuffer(GraphicsDevice& device)
{
	if (cb)
	{
		cb->Update(device, property);
	}
}

template<typename VS, typename PS>
	requires std::is_base_of_v<VertexShader, VS>&& std::is_base_of_v<PixelShader, PS>
inline void MaterialImpl<VS, PS>::Bind(GraphicsDevice& device)
{
	shader.Bind(device);
	if (cb)
	{
		cb->Bind(device);
	}
	if (texture)
	{
		texture->Bind(device);
	}
}


#endif
