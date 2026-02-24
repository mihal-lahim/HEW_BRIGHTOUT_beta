#pragma once

#include "ScriptComponent.h"
#include "Vector3.h"
#include <string>
#include <memory>
#include <unordered_map>

class Mesh;
class MeshRenderer;
class Health;
class GameObject;

class HPBarUI : public ScriptComponent
{
public:
	HPBarUI() = default;
	void Start() override;
	void Update() override;

	// 設定可能な公開プロパティ
	std::wstring backgroundTexture = L"texture/BRIGHTOUT_battery_kara.png";
	std::wstring fillTexture = L"texture/BRIGHTOUT_battery_ge-ji.png";
	Vector3 position{ 50.0f, 800.0f, 0.0f };
	Vector3 scale{ 400.0f, 400.0f, 1.0f };
	std::string vsPath = "UiVS.cso";
	std::string psPath = "UiPS.cso";

private:
	// ランタイム参照
	MeshRenderer* m_bgRenderer = nullptr;
	MeshRenderer* m_fillRenderer = nullptr;
	GameObject* m_bgObj = nullptr;
	GameObject* m_fillObj = nullptr;
	Health* m_playerHealth = nullptr;

	// 前回のパーセンテージ（更新コスト低減）
	int m_lastPercent = -1;
	std::unordered_map<int, std::shared_ptr<Mesh>> m_meshCache;

	// 元スケールを保持
	Vector3 m_originalScale{};
};