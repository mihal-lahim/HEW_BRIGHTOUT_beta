#include "Scene.h"
#include "Time.h"
#include "GameContext.h"
#include "PhysicsSystem.h"
#include "ScriptComponent.h"
#include "Prefab.h"
#include "GameObject.h"


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
		comp->InitializeByContext();
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
		scriptComp->Awake();
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
			comp->FinalizeByContext();
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
	for (auto& poolBase : m_scriptComponents)
	{
		auto* scriptCompPool = static_cast<ObjectPool<ScriptComponent>*>(poolBase.get());

		if (!scriptCompPool) continue;

		// プール内のすべてのスクリプトコンポーネントを更新
		for (size_t i = 0; i < scriptCompPool->Size(); ++i)
		{
			ScriptComponent* scriptComp = scriptCompPool->Get(static_cast<uint32_t>(i));
			if (scriptComp && scriptComp->IsEnable() && !scriptComp->m_hasStarted)
			{
				scriptComp->Start();
				scriptComp->m_hasStarted = true;
			}
		}
	}
}

void Scene::PreUpdate()
{
	// スクリプトコンポーネントをPreUpdate呼び出し
	for (auto& poolBase : m_scriptComponents)
	{
		auto* scriptCompPool = static_cast<ObjectPool<ScriptComponent>*>(poolBase.get());

		if (!scriptCompPool) continue;

		// プール内のすべてのスクリプトコンポーネントを更新
		for (size_t i = 0; i < scriptCompPool->Size(); ++i)
		{
			ScriptComponent* scriptComp = scriptCompPool->Get(static_cast<uint32_t>(i));
			if (scriptComp && scriptComp->IsEnable())
			{
				scriptComp->PreUpdate();
			}
		}
	}
}

void Scene::Update()
{
	// スクリプトコンポーネントをUpdate呼び出し
	for (auto& poolBase : m_scriptComponents)
	{
		auto* scriptCompPool = static_cast<ObjectPool<ScriptComponent>*>(poolBase.get());

		if (!scriptCompPool) continue;

		// プール内のすべてのスクリプトコンポーネントを更新
		for (size_t i = 0; i < scriptCompPool->Size(); ++i)
		{
			ScriptComponent* scriptComp = scriptCompPool->Get(static_cast<uint32_t>(i));
			if (scriptComp && scriptComp->IsEnable())
			{
				scriptComp->Update();
			}
		}
	}
}

void Scene::PostUpdate()
{
	// スクリプトコンポーネントをPostUpdate呼び出し
	for (auto& poolBase : m_scriptComponents)
	{
		auto* scriptCompPool = static_cast<ObjectPool<ScriptComponent>*>(poolBase.get());

		if (!scriptCompPool) continue;

		// プール内のすべてのスクリプトコンポーネントを更新
		for (size_t i = 0; i < scriptCompPool->Size(); ++i)
		{
			ScriptComponent* scriptComp = scriptCompPool->Get(static_cast<uint32_t>(i));
			if (scriptComp && scriptComp->IsEnable())
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
		m_gameContext.physicsSystem->PhysicsUpdate(*this, (float)Time::FixedDeltaTime());
	}

	// シーンの更新サイクルを実行
	Cycle();
}

void Scene::Enable()
{
	auto* gameObjectPool = static_cast<ObjectPool<GameObject>*>(m_gameObjects.get());
	if (!gameObjectPool) return;

	// プール内のすべてのゲームオブジェクトを有効化
	for (size_t i = 0; i < gameObjectPool->Size(); ++i)
	{
		GameObject* gameObject = gameObjectPool->Get(static_cast<uint32_t>(i));
		if (gameObject)
		{
			for (auto* component : gameObject->m_components)
			{
				component->OnEnable();
			}
		}
	}
}

void Scene::Disable()
{
	auto* gameObjectPool = static_cast<ObjectPool<GameObject>*>(m_gameObjects.get());
	if (!gameObjectPool) return;

	// プール内のすべてのゲームオブジェクトを無効化
	for (size_t i = 0; i < gameObjectPool->Size(); ++i)
	{
		GameObject* gameObject = gameObjectPool->Get(static_cast<uint32_t>(i));
		if (gameObject)
		{
			for (auto* component : gameObject->m_components)
			{
				component->OnDisable();
			}
		}
	}
}

GameObject* Scene::CreateGameObject()
{
	// 新しいゲームオブジェクトを作成
	GameObject* newGameObject = new GameObject();
	newGameObject->m_scene = this;
	newGameObject->m_transform = CreateComponent<Transform>();
	newGameObject->m_gameContext = m_gameContext;
	// 作成保留キューに追加
	m_pendingAddGameObjects.push(newGameObject);

	if (!m_gameObjects)
	{
		m_gameObjects = std::make_unique<ObjectPool<GameObject>>();
	}
	return newGameObject;
}

GameObject* Scene::Instantiate(Prefab& prefab)
{
	// 新しいゲームオブジェクトを作成
	GameObject* newGameObject = CreateGameObject();
	prefab.Instantiate(*newGameObject);
	return newGameObject;
}


void Scene::DestroyGameObject(GameObject* gameObject)
{
	m_pendingDestroyGameObjects.push(gameObject->m_allocationID);
}

void Scene::DestroyComponent(Component* component)
{
	if (!component) return;

	// コンポーネントプールの型IDを取得
	uint32_t typeID = component->m_typeID;

	// 破棄保留リストに追加
	DestroyPending pending{ component->m_allocationID, typeID };

	// スクリプトコンポーネントかどうかで振り分け
	if (m_scriptComponentTypeIDs.find(typeID) != m_scriptComponentTypeIDs.end())
	{
		m_pendingDestroyScriptComponents.push(pending);
	}
	else
	{
		m_pendingDestroyComponents.push(pending);
	}
}
