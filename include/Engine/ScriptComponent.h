
#ifndef SCRIPT_COMPONENT_H
#define SCRIPT_COMPONENT_H

#include "Component.h"

class ScriptComponent : public Component
{
public:
	// 各種更新メソッド
	virtual void Awake() {};
	virtual void Start() {};
	virtual void PreUpdate() {};
	virtual void Update() {};
	virtual void PostUpdate() {};
	virtual void OnDestroy() {};
private:
	// スタート済みフラグ
	bool m_hasStarted = false;

	friend class Scene;
};



#endif
