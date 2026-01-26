#include "ObjectManager.h"
#include "GameObject.h"
#include "Component.h"



void ObjectManager::Initialize()
{
    m_GameObjects.clear();
	m_Components.clear();
	m_ComponentMap.clear();
	m_PendingGameObjects.clear();
	m_PendingComponents.clear();
}

void ObjectManager::Finalize()
{
    m_GameObjects.clear();
	m_Components.clear();
	m_ComponentMap.clear();
	m_PendingGameObjects.clear();
	m_PendingComponents.clear();
}


void ObjectManager::Start()
{
	for (auto& obj : m_GameObjects) 
	{
		if (obj->m_IsActive && !obj->m_HasStarted)
		{
			obj->Start();
			obj->m_HasStarted = true;
		}
	}

	for (auto& comp : m_Components) 
	{
		if (comp->m_IsActive && !comp->m_HasStarted)
		{
			comp->Start();
			comp->m_HasStarted = true;
		}
	}
}

void ObjectManager::PreUpdate()
{
	for (auto& obj : m_GameObjects) {
		if(obj->m_IsActive) obj->PreUpdate();
	}

	for (auto& comp : m_Components) {
		if(comp->m_IsActive) comp->PreUpdate();
	}
}

void ObjectManager::Update()
{
	for (auto& obj : m_GameObjects) {
		if (obj->m_IsActive) obj->Update();
	}

	for (auto& comp : m_Components) {
		if (comp->m_IsActive) comp->Update();
	}
}

void ObjectManager::PostUpdate()
{
	for (auto& obj : m_GameObjects) {
		if (obj->m_IsActive) obj->PostUpdate();
	}

	for (auto& comp : m_Components) {
		if (comp->m_IsActive) comp->PostUpdate();
	}

    DestroyGameObjects();
    DestroyComponents();
    AddPendingGameObjects();
    AddPendingComponents();
}


void ObjectManager::RegisterGameObject(GameObject* obj)
{
	if (!obj) return;

	// オブジェクトのオーナーを設定
	obj->m_ObjectManager = this;

	// 保留リストに追加
	m_PendingGameObjects.push_back(obj);
}


void ObjectManager::RegisterComponent(Component* comp)
{
	if (!comp) return;

	// コンポーネントのオーナーを設定
	comp->m_ObjectManager = this;

	// 保留リストに追加
	m_PendingComponents.push_back(comp);
}


void ObjectManager::AddPendingGameObjects()
{
	// 保留中のゲームオブジェクトをメインリストに追加
    for (auto* obj : m_PendingGameObjects)
    {
		// オブジェクトのオーナーを設定
		obj->m_ObjectManager = this;

		// IDを設定
		obj->m_ID = (ObjectID)m_GameObjects.size();


		// Componentマップ用の空セットを追加
        m_ComponentMap.push_back({});

        m_GameObjects.push_back(std::unique_ptr<GameObject>(obj));

		// Awakeを呼び出し
		obj->Awake();
    }

	m_PendingGameObjects.clear();
}


void ObjectManager::AddPendingComponents()
{
	// 保留中のコンポーネントをメインリストに追加
    for (auto* comp : m_PendingComponents)
    {
		// コンポーネントのオーナーを設定
		comp->m_ObjectManager = this;

		// IDを設定
		comp->m_ID = (ObjectID)m_Components.size();

		// GameObjectごとのコンポーネントマップに登録
		m_ComponentMap.at(comp->gameObject()->m_ID).push_back(comp);

        m_Components.push_back(std::unique_ptr<Component>(comp));

		// Awakeを呼び出し
		comp->Awake();
    }

	m_PendingComponents.clear();
}


void ObjectManager::DestroyGameObjects()
{
	// 後ろから削除して安全に処理
	for (size_t i = m_GameObjects.size(); i-- > 0;)
	{
		if (m_GameObjects.at(i)->CanDestroy())
			DestroyGameObjectByID(m_GameObjects.at(i)->m_ID);
	}
}


void ObjectManager::DestroyComponents()
{
	// 後ろから削除して安全に処理
	for (size_t i = m_GameObjects.size(); i-- > 0;)
	{
		if (m_Components.at(i)->CanDestroy())
			DestroyComponentByID(m_Components.at(i)->m_ID);
	}
}


void ObjectManager::DestroyGameObjectByID(ObjectID id)
{
	// コピーを作成してから削除
	auto compList = m_ComponentMap.at(id);
	for (auto* comp : compList)
		DestroyComponentByID(comp->m_ID);

	std::swap(m_GameObjects.at(id), m_GameObjects.back());
	std::swap(m_ComponentMap.at(id), m_ComponentMap.back());

	m_GameObjects.pop_back();
	m_ComponentMap.pop_back();


	m_GameObjects.at(id)->m_ID = id;
}


void ObjectManager::DestroyComponentByID(ObjectID id)
{
	// m_ComponentMapから該当コンポーネントを削除
	DestroyComponentMap(id);

	// Componentリストで対象IDと最後尾を入れ替え
	std::swap(m_Components.at(id), m_Components.back());

	// 最後尾を削除
	m_Components.pop_back();

	// 入れ替えたComponentのIDを更新（リストが空でない場合のみ）
	if (id < m_Components.size())
		m_Components.at(id)->m_ID = id;
}


void ObjectManager::DestroyComponentMap(ObjectID componentID)
{
	// 削除対象のコンポーネントを取得
	Component* targetComp = m_Components.at(componentID).get();
	ObjectID gameObjectID = targetComp->gameObject()->m_ID;

	// m_ComponentMapから該当コンポーネントを削除
	auto& compList = m_ComponentMap.at(gameObjectID);

	// イテレータを取得して削除
	auto it = std::find(compList.begin(), compList.end(), targetComp);

	if (it == compList.end()) return;

	std::swap(*it, compList.back());
	compList.pop_back();
}
