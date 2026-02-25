#pragma once

#include "ScriptComponent.h"
#include "Vector3.h"
#include <string>
#include <memory>

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

	// 従来の単一位置（後方互換）
	Vector3 position{ 50.0f, 800.0f, 0.0f };

	// 背景とゲージの位置を個別に調整可能
	Vector3 backgroundPosition{ 50.0f, 800.0f, 0.0f };
	Vector3 fillPosition{ 40.0f, 800.0f, 0.0f }; // Inspector で個別指定が可能

	// スケール：共通既定（後方互換）
	Vector3 scale{ 400.0f, 400.0f, 1.0f };

	// 背景とゲージを個別に調整
	Vector3 backgroundScale{ 400.0f, 400.0f, 1.0f };
	Vector3 fillScale{ 400.0f, 400.0f, 1.0f }; 

	std::string vsPath = "UiVS.cso";
	std::string psPath = "UiPS.cso";

	Vector3 m_fillOriginalPosition{};

	// ランタイムで位置を変更するための API
	void SetBackgroundPosition(const Vector3& pos);
	void SetFillPosition(const Vector3& pos);

private:
	// ランタイム参照
	MeshRenderer* m_bgRenderer = nullptr;
	MeshRenderer* m_fillRenderer = nullptr;
	GameObject* m_bgObj = nullptr;
	GameObject* m_fillObj = nullptr;
	Health* m_playerHealth = nullptr;

	// 前回のパーセンテージ
	int m_lastPercent = -1;

	// フル幅メッシュ
	std::shared_ptr<Mesh> m_fullMesh;

	std::shared_ptr<Mesh> m_fillMesh;

	// 元スケールを保持
	Vector3 m_bgOriginalScale{};
	Vector3 m_fillOriginalScale{};

	// Start 時点の背景右端に対する fill の右端の相対オフセット
	float m_relRightX = 0.0f;
};