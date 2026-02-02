#ifndef MATERIAL_H
#define MATERIAL_H

#include "Shader.h"

class Material2D
{
public:
	VertexShader2D* VertexShader = nullptr;
	PixelShader2D* PixelShader = nullptr;
};

class Material3D
{
public:
	VertexShader3D* VertexShader = nullptr;
	PixelShader3D* PixelShader = nullptr;
};

#endif
