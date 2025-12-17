//////////////////////////////////
// model.h
//Author: hiroshi kasiwagi
//Date:2025/12/17
//////////////////////////////////
#ifndef MODEL_H
#define MODEL_H
#include <unordered_map>
#include <d3d11.h>
#include <DirectXMath.h>
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/matrix4x4.h"
#pragma comment (lib, "assimp-vc143-mt.lib")
#include <vector>
#include <string>

struct Bone
{
    std::string name;
    int parent = -1;

    DirectX::XMMATRIX offset = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX local = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();
};

struct MODEL
{
    const aiScene* AiScene = nullptr;

    ID3D11Buffer** VertexBuffer = nullptr;
    ID3D11Buffer** IndexBuffer = nullptr;

    std::unordered_map<std::string, ID3D11ShaderResourceView*> Texture;

    std::vector<Bone> Bones;
    std::unordered_map<std::string, int> BoneIndexMap;
};



MODEL* ModelLoad(const char* FileName, float scale = 1.0f,bool isBlender = false);
void ModelRelease(MODEL* model);

void ModelDraw(const MODEL* model, const DirectX::XMMATRIX& mtxWorld);
#endif // MODEL_H

