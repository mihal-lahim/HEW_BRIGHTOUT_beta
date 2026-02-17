#include "PowerPlant.h"
#include "GameObject.h"

// GameObjectとその全子孫を再帰的にSetActiveする
static void SetActiveRecursive(GameObject* obj, bool active)
{
	if (!obj) return;
	obj->SetActive(active);
	for (auto* child : obj->GetChildren())
	{
		SetActiveRecursive(child, active);
	}
}

void PowerPlant::Start()
{
	// コンポーネント初期化後に復旧モデルを非表示にする
	SetActiveRecursive(restoredModel, false);
}

void PowerPlant::Restore()
{
	if (m_isRestored)
		return;

	m_isRestored = true;

	// 故障モデルを非表示にし、復旧モデルを表示
	SetActiveRecursive(brokenModel, false);
	SetActiveRecursive(restoredModel, true);
}

bool PowerPlant::IsInRange(const Vector3& position) const
{
	Vector3 plantPos = gameObject().transform().position();
	Vector3 diff = position - plantPos;
	float distSq = diff.x * diff.x + diff.z * diff.z;
	return distSq <= m_interactRadius * m_interactRadius;
}
