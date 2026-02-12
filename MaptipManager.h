#pragma once
#include <vector>
#include <unordered_map>
#include <DirectXMath.h>
#include <string>

struct MODEL;

class MapTipManager
{
public:
    static MapTipManager& Instance();

    bool LoadFromCSV(const std::string& csvPath);
    void RenderAll();
    void Clear();

private:
    MapTipManager();
    ~MapTipManager();

    MapTipManager(const MapTipManager&) = delete;
    MapTipManager& operator=(const MapTipManager&) = delete;

    struct InstanceData
    {
        MODEL* model = nullptr;
        DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();

        DirectX::XMFLOAT3 position{ 0,0,0 };
        DirectX::XMFLOAT3 rotation{ 0,0,0 }; 
        float scale = 1.0f;
    };

    std::vector<InstanceData> m_instances;
    std::unordered_map<int, MODEL*> m_modelCache;

    MODEL* GetModelByID(int id);
};