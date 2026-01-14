#ifndef COMPONENT_H
#define COMPONENT_H

#include <cstdint>

class GameObject;

class Component
{
private:
	// コンポーネントGameObjectごとの一意なID
	uint64_t m_ID = 0;
	// 所有者のゲームオブジェクトポインタ
	GameObject* m_Owner = nullptr;
	// コンポーネントがアクティブかどうか
	bool m_IsActive = true;
	// コンポーネントが破壊可能かどうか
	bool m_CanDestroy = false;
public:
	Component() = default;
	virtual ~Component() = default;


	// 所有者のゲームオブジェクト取得メソッド
	GameObject* GetOwner() const { return m_Owner; }

	// アクティブフラグの取得・設定メソッド
	void SetActive(bool isActive) { m_IsActive = isActive; }

	virtual void Start() {};
	virtual void PreUpdate(double) {};
	virtual void Update(double) {};
	virtual void PostUpdate(double) {};

	// 破壊可能フラグの取得・設定メソッド
	void CanDestroy() { m_CanDestroy = true; }
	void Destroy() { m_CanDestroy = true; }


	friend class GameObject;
};




#endif
