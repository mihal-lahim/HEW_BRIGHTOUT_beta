#pragma once

#include "ScriptComponent.h"
#include "Vector3.h"
#include <memory>

class Mesh;
class MeshRenderer;
class GameObject;
class PowerPlant;

class PowerPlantUI : public ScriptComponent
{
public:
	PowerPlantUI() = default;
	void Start() override;
	void Update() override;

	// テクスチャパス（必要に応じて編集）
	std::wstring backgroundTexture = L"texture/ui_charge_bg.png";
	std::wstring fillTexture = L"texture/ui_charge_fill.png";
	Vector3 localOffset{ 3.0f, 3.0f, 0.0f }; // プラント基準のオフセット位置（ワールド単位）
	Vector3 uiScale{ 1.5f, 4.0f, 1.0f }; // 幅，高さ（ワールドスケール）

private:
	MeshRenderer* m_bgRenderer = nullptr;
	MeshRenderer* m_fillRenderer = nullptr;
	GameObject* m_bgObj = nullptr;
	GameObject* m_fillObj = nullptr;

	PowerPlant* m_powerPlant = nullptr;

	// ビルボード用クワッドメッシュ
	std::shared_ptr<Mesh> m_billboardQuad;

	// 元スケール保持
	Vector3 m_originalScale{};
};