#pragma once

#include "ScriptComponent.h"
#include "Vector3.h"
#include <vector>
#include <memory>
#include <string>

class Mesh;
class MeshRenderer;
class GameObject;

/// 発電所の復旧数 / 全体数 を画面左上に表示する UI コンポーネント
/// 「現在数 / 9」の形式で、Number_ver2.png から数字を切り出して表示する。
/// 座標・スケールはシーン側から自由に調整可能。
class PowerPlantCountUI : public ScriptComponent
{
public:
	PowerPlantCountUI() = default;

	void Start() override;
	void Update() override;

	// --- シーンから調整可能なパラメータ ---

	// アイコン画像パス（発電所のアイコン）
	std::wstring iconTexturePath = L"texture/powerplant_icon.png";

	// アイコンの位置・スケール（ピクセル座標）
	Vector3 iconPosition  = Vector3(50.0f, 50.0f, 0.0f);
	Vector3 iconScale     = Vector3(800.0f, 800.0f, 1.0f);

	// 現在数（分子）の位置・スケール
	Vector3 currentDigitPosition = Vector3(140.0f, 40.0f, 0.0f);
	Vector3 currentDigitScale    = Vector3(40.0f, 60.0f, 1.0f);

	// 最大数（分母）の位置・スケール
	Vector3 totalDigitPosition = Vector3(220.0f, 40.0f, 0.0f);
	Vector3 totalDigitScale    = Vector3(40.0f, 60.0f, 1.0f);

	// 発電所の総数
	int totalPlantCount = 9;

private:
	// 0..9 の切り出しメッシュ
	std::vector<std::shared_ptr<Mesh>> m_digitMeshes;

	// 現在数の桁 MeshRenderer / GameObject
	MeshRenderer* m_currentDigitRenderer = nullptr;
	GameObject*m_currentDigitObject   = nullptr;

	// 最大数の桁 MeshRenderer / GameObject
	MeshRenderer* m_totalDigitRenderer = nullptr;
	GameObject*   m_totalDigitObject   = nullptr;

	// アイコン GameObject
	GameObject* m_iconObject = nullptr;

	// スラッシュ GameObject
	GameObject* m_slashObject = nullptr;

	// 前回表示した値（不要な更新を避ける）
	int m_prevCurrentCount = -1;
};
