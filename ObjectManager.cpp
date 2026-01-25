#include "ObjectManager.h"
#include "GameObject.h"
#include "camera.h"
#include "MeshRenderer.h"
#include "Collider.h"
#include "RigidBody.h"
#include <map>

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


void ObjectManager::Draw() const
{
	// カメラを取得
	auto cameraVec = GetComponents<Camera>();

	// カメラを優先度順にマップに格納
	std::map<int, std::vector<Camera*>> cameraMap;
    for (auto* camera : cameraVec)
		cameraMap[camera->Priority].push_back(camera);

	auto meshrenderers = GetComponents<MeshRenderer>();

	// 優先度順に描画
    for (auto& [priority, cameras] : cameraMap)
    {
		// 同一優先度のカメラごとに描画
        for (auto* camera : cameras)
        {
            if (camera->IsActive() == false) continue;

            // カメラ行列を設定
            camera->SetMatrix();

			// メッシュレンダラーを描画
            for (auto* meshrenderer : meshrenderers)
            {
                if (meshrenderer->IsActive())
                    meshrenderer->Render();
			}
        }
    }
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
		obj->m_ID = m_GameObjects.size();


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
		comp->m_ID = m_Components.size();

		// GameObjectごとのコンポーネントマップに登録
		m_ComponentMap.at(comp->GameObject()->m_ID).push_back(comp);

        m_Components.push_back(std::unique_ptr<Component>(comp));

		// Awakeを呼び出し
		comp->Awake();
    }

	m_PendingComponents.clear();
}


void ObjectManager::DestroyGameObjects()
{
    for (auto& obj : m_GameObjects)
        if (obj->CanDestroy())
			DestroyComponentByID(obj->m_ID);
}


void ObjectManager::DestroyComponents()
{
    for (auto& comp : m_Components)
        if (comp->CanDestroy())
			DestroyComponentByID(comp->m_ID);
}


void ObjectManager::DestroyGameObjectByID(ObjectID id)
{
	// 所持しているコンポーネントをすべて破棄
	for (auto* comp : m_ComponentMap.at(id))
		DestroyComponentByID(comp->m_ID);

	// GameObjectリストとコンポーネントマップで対象IDと最後尾を入れ替え
	std::swap(m_GameObjects.at(id), m_GameObjects.back());
	std::swap(m_ComponentMap.at(id), m_ComponentMap.back());

	// 最後尾を削除
	m_GameObjects.pop_back();
	m_ComponentMap.pop_back();

	// 入れ替えたGameObjectのIDを更新
	m_GameObjects.at(id)->m_ID = id;
}


void ObjectManager::DestroyComponentByID(ObjectID id)
{
	// Componentリストで対象IDと最後尾を入れ替え
	std::swap(m_Components.at(id), m_Components.back());

	// 最後尾を削除
	m_Components.pop_back();

	// 入れ替えたComponentのIDを更新
	m_Components.at(id)->m_ID = id;
}
