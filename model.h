#pragma once

#include <unordered_map>
#pragma once

#include <unordered_map>
#include <d3d11.h>
#include <DirectXMath.h>

#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#include "GraphicsDevice.h"
#pragma comment (lib, "assimp-vc143-mt.lib")



struct MODEL
{
	const aiScene* AiScene = nullptr;

	ID3D11Buffer** VertexBuffer{};
	ID3D11Buffer** IndexBuffer{};

	std::unordered_map<std::string, ID3D11ShaderResourceView*> Texture;
};

void ModelInitialize(GraphicsDevice* device);

MODEL* ModelLoad(const char* FileName, float scale);
void ModelRelease(MODEL* model);

void ModelDraw(const MODEL* model, const DirectX::XMMATRIX& mtxWorld);

static void LogSceneBounds(const MODEL* model);
