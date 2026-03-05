#ifndef BILLBOARD_PREFAB_H
#define BILLBOARD_PREFAB_H

#include "Prefab.h"
#include "Billboard.h"
#include "GameObject.h"
#include "Renderer.h"
#include "ResourceSystem.h"

class BillboardPrefab : public Prefab
{
public:
	BillboardPrefab() = default;
	virtual ~BillboardPrefab() = default;
	// インスタンス化メソッド
	virtual void Instantiate(GameObject& gameObject) override
	{
		auto* renderer = gameObject.AddComponent<MeshRenderer>();
		renderer->material.texturePath = L"texture/crystal.png";
		// Billboard::Awake() で Quad メッシュとシェーダーが自動設定される
		gameObject.AddComponent<Billboard>();
	}
};




#endif
