#include "Scene.h"
#include "Time.h"
#include "GameContext.h"
#include "PhysicsSystem.h"
#include "RenderingSystem.h"
#include "ScriptComponent.h"
#include "RigidBody.h"

void Scene::AddPendingGameObjectsProcess()
{
	// 保留中のゲームオブジェクトをすべて追加
	while (!m_pendingAddGameObjects.empty())
	{
		auto* obj = m_pendingAddGameObjects.front();
		static_cast<ObjectPool<GameObject>*>(m_gameObjects.get())->Register(obj);
		m_pendingAddGameObjects.pop();
	}
}

void Scene::AddPendingComponentsProcess()
{
	// 保留中のコンポーネントをすべて追加
	while (!m_pendingAddComponents.empty())
	{
		const auto& pending = m_pendingAddComponents.front();
		auto* compPool = static_cast<ObjectPool<Component>*>(m_components.at(pending.TypeID).get());
		auto* comp = static_cast<Component*>(pending.ObjectPtr);
		compPool->Register(comp);
		comp->ContextInitialize();
		m_pendingAddComponents.pop();
	}
}

void Scene::AddPendingScriptComponentsProcess()
{
	// 保留中のスクリプトコンポーネントをすべて追加
	while (!m_pendingAddScriptComponents.empty())
	{
		const auto& pending = m_pendingAddScriptComponents.front();
		auto* scriptCompPool = static_cast<ObjectPool<ScriptComponent>*>(m_scriptComponents.at(pending.TypeID).get());
		auto* scriptComp = static_cast<ScriptComponent*>(pending.ObjectPtr);
		scriptCompPool->Register(scriptComp);
		scriptComp->ContextInitialize();
		scriptComp->Awake();
		m_preStartScriptComponents.push({ pending.ObjectPtr->m_allocationID, pending.TypeID });
		m_pendingAddScriptComponents.pop();
	}
}

void Scene::DestroyPendingGameObjectsProcess()
{
	// 保留中のゲームオブジェクトをすべて破棄
	while (!m_pendingDestroyGameObjects.empty())
	{
		const auto& idx = m_pendingDestroyGameObjects.front();
		auto* objPool = static_cast<ObjectPool<GameObject>*>(m_gameObjects.get());
		GameObject* obj = objPool->Get(idx);
		if (obj)
		{
			objPool->Destroy(idx);
		}
		m_pendingDestroyGameObjects.pop();
	}
}

void Scene::DestroyPendingComponentsProcess()
{
	// 保留中のコンポーネントをすべて破棄
	while (!m_pendingDestroyComponents.empty())
	{
		const auto& pending = m_pendingDestroyComponents.front();
		auto* compPool = static_cast<ObjectPool<Component>*>(m_components.at(pending.TypeID).get());
		Component* comp = compPool->Get(pending.AllocationID);
		if (comp)
		{
			compPool->Destroy(pending.AllocationID);
		}
		m_pendingDestroyComponents.pop();
	}
}

void Scene::DestroyPendingScriptComponentsProcess()
{
	// 保留中のスクリプトコンポーネントをすべて破棄
	while (!m_pendingDestroyScriptComponents.empty())
	{
		const auto& pending = m_pendingDestroyScriptComponents.front();
		auto* scriptCompPool = static_cast<ObjectPool<ScriptComponent>*>(m_scriptComponents.at(pending.TypeID).get());
		ScriptComponent* scriptComp = scriptCompPool->Get(pending.AllocationID);
		if (scriptComp)
		{
			scriptComp->OnDestroy();
			scriptCompPool->Destroy(pending.AllocationID);
		}
		m_pendingDestroyScriptComponents.pop();
	}
}

void Scene::Start()
{
	// スクリプトコンポーネントのStart呼び出し
	while (!m_preStartScriptComponents.empty())
	{
		const auto& desc = m_preStartScriptComponents.front();
		auto* scriptCompPool = static_cast<ObjectPool<ScriptComponent>*>(m_scriptComponents.at(desc.TypeID).get());
		ScriptComponent* scriptComp = scriptCompPool->Get(desc.AllocationID);
		scriptComp->Start();
		m_preStartScriptComponents.pop();
	}
}

void Scene::PreUpdate()
{
	// スクリプトコンポーネントのPreUpdate呼び出し
	for (auto& poolBase : m_scriptComponents)
	{
		auto* scriptCompPool = static_cast<ObjectPool<ScriptComponent>*>(poolBase.get());
		// プール内のすべてのスクリプトコンポーネントを更新
		for (size_t i = 0; i < scriptCompPool->Size(); ++i)
		{
			ScriptComponent* scriptComp = scriptCompPool->Get(static_cast<uint32_t>(i));
			if (scriptComp && scriptComp->IsActive())
			{
				scriptComp->PreUpdate();
			}
		}
	}
}

void Scene::Update()
{
	// スクリプトコンポーネントのUpdate呼び出し
	for (auto& poolBase : m_scriptComponents)
	{
		auto* scriptCompPool = static_cast<ObjectPool<ScriptComponent>*>(poolBase.get());
		// プール内のすべてのスクリプトコンポーネントを更新
		for (size_t i = 0; i < scriptCompPool->Size(); ++i)
		{
			ScriptComponent* scriptComp = scriptCompPool->Get(static_cast<uint32_t>(i));
			if (scriptComp && scriptComp->IsActive())
			{
				scriptComp->Update();
			}
		}
	}
}

void Scene::PostUpdate()
{
	// スクリプトコンポーネントのPostUpdate呼び出し
	for (auto& poolBase : m_scriptComponents)
	{
		auto* scriptCompPool = static_cast<ObjectPool<ScriptComponent>*>(poolBase.get());
		// プール内のすべてのスクリプトコンポーネントを更新
		for (size_t i = 0; i < scriptCompPool->Size(); ++i)
		{
			ScriptComponent* scriptComp = scriptCompPool->Get(static_cast<uint32_t>(i));
			if (scriptComp && scriptComp->IsActive())
			{
				scriptComp->PostUpdate();
			}
		}
	}
}

void Scene::UpdateScene()
{
	Time::BeginFrame();

	// 物理演算の固定更新を処理
	while (Time::HasFixedStep())
	{
		Time::ConsumeFixedStep();
		auto rigidbodies = GetComponents<RigidBody>();
		m_gameContext->physicsSystem->PhysicsUpdate(*this, (float)Time::FixedDeltaTime());
	}

	// シーンの更新サイクルを実行
	Cycle();
}

GameObject* Scene::CreateGameObject()
{
	GameObject* obj = new GameObject();
	m_pendingAddGameObjects.push(obj);

	return obj;
}

void Scene::DestroyGameObject(GameObject* gameObject)
{
	m_pendingDestroyGameObjects.push(gameObject->m_allocationID);
}
