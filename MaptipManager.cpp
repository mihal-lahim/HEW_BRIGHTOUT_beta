#include "MapTipManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "model.h"
#include "shader3d.h"
#include <windows.h>
#include <cstdio>

using namespace DirectX;

MapTipManager& MapTipManager::Instance()
{
    static MapTipManager s;
    return s;
}

MapTipManager::MapTipManager() = default;

MapTipManager::~MapTipManager()
{
    Clear();
}

//============================================================
// モデル取得（キャッシュ付き）
//============================================================
MODEL* MapTipManager::GetModelByID(int id)
{
    auto it = m_modelCache.find(id);
    if (it != m_modelCache.end())
        return it->second;

    const char* path = nullptr;
    switch (id)
    {
    case 0: path = "model/ground.fbx";       break;
    case 1: path = "model/karaoke.fbx";      break;
    case 2: path = "model/house.fbx";        break;
    case 3: path = "model/convenience.fbx";  break;
    case 4: path = "model/powerplant.fbx";   break;
    case 5: path = "model/powerplant_broken.fbx";   break;
	case 6: path = "model/mansion01.fbx";    break;
    case 7: path = "model/mansion02.fbx";    break;
    case 8: path = "model/wacdonald.fbx";    break;
	case 9: path = "model/apartment.fbx";    break;
	case 10: path = "model/clocktower.fbx";    break;
	case 11: path = "model/dentyuu.fbx";    break;
	case 12: path = "model/special_dentyuu.fbx";    break;
	case 13: path = "model/special_densen.fbx";    break;

       
    default:
        OutputDebugStringA("[MapTip] Invalid ID\n");
        return nullptr;
    }

    MODEL* model = ModelLoad(path, 1.0f);
    if (!model)
        return nullptr;

    m_modelCache.emplace(id, model);
    return model;
}

//============================================================
// CSV 読み込み
//============================================================
bool MapTipManager::LoadFromCSV(const std::string& csvPath)
{
    Clear();

    std::ifstream ifs(csvPath);
    if (!ifs.is_open())
        return false;

    std::string line;
    bool isFirstLine = true;

    while (std::getline(ifs, line))
    {
		// 空行スキップ
        if (line.empty())
            continue;

        // ヘッダスキップ
        if (isFirstLine)
        {
            isFirstLine = false;
            continue;
        }

        std::stringstream ss(line);

        std::string sid, sx, sy, sz, srx, sry, srz, sscale;

        std::getline(ss, sid, ',');
        std::getline(ss, sx, ',');
        std::getline(ss, sy, ',');
        std::getline(ss, sz, ',');
        std::getline(ss, srx, ',');
        std::getline(ss, sry, ',');
        std::getline(ss, srz, ',');
        std::getline(ss, sscale, ',');

        int   id = std::stoi(sid);
        float x = std::stof(sx);
        float y = std::stof(sy);
        float z = std::stof(sz);
        float rotX = XMConvertToRadians(std::stof(srx));
        float rotY = XMConvertToRadians(std::stof(sry));
        float rotZ = XMConvertToRadians(std::stof(srz));
        float scale = std::stof(sscale);

        MODEL* model = GetModelByID(id);
        if (!model)
            continue;

        XMMATRIX mScale = XMMatrixScaling(scale, scale, scale);
        XMMATRIX mRot = XMMatrixRotationRollPitchYaw(rotX, rotY, rotZ);
        XMMATRIX mTrans = XMMatrixTranslation(x, y, z);

        InstanceData inst;
        inst.model = model;
        inst.world = mScale * mRot * mTrans;

        inst.position = { x, y, z };
        inst.rotation = { rotX, rotY, rotZ };
        inst.scale = scale;

        char log[512];
        sprintf_s(
            log,
            "[MapTip] id=%d pos(%.2f, %.2f, %.2f) rot(rad)(%.2f, %.2f, %.2f) scale=%.2f\n",
            id, x, y, z, rotX, rotY, rotZ, scale
        );
        OutputDebugStringA(log);

        m_instances.push_back(inst);
    }

    return true;
}

void MapTipManager::RenderAll()
{
    if (m_instances.empty())
        return;
    Shader3d_Begin();

    Shader3d_SetLight(
        { 0.2f, 0.2f, 0.2f, 1.0f },
        { 1.0f, 1.0f, 1.0f, 1.0f },
        { 0.0f, -1.0f, 0.5f, 0.0f }  
    );

    for (auto& inst : m_instances)
    {
        if (!inst.model)
            continue;

        Shader3d_SetWorldMatrix(inst.world);
        ModelDraw(inst.model, inst.world);
    }
}

//============================================================
// 破棄
//============================================================
void MapTipManager::Clear()
{
    m_instances.clear();

    for (auto& kv : m_modelCache)
    {
        if (kv.second)
            ModelRelease(kv.second);
    }
    m_modelCache.clear();
}
