#ifndef BILLBOARD_PREFAB_H
#define BILLBOARD_PREFAB_H

#include "Prefab.h"
#include "Billboard.h"
#include "GameObject.h"
#include "SpriteRenderer.h"
#include "ResourceSystem.h"

class BillboardPrefab : public Prefab
{
public:
	BillboardPrefab() = default;
	virtual ~BillboardPrefab() = default;
	// インスタンス化メソッド
	virtual void Instantiate(GameObject& gameObject) override
	{
		auto* renderer = gameObject.AddComponent<SpriteRenderer>();
		renderer->SetTexture(gameObject.resource().Load<Texture>(L"texture/crystal.png"));
		gameObject.AddComponent<Billboard>();
	}
};




#endif
