#include "ObjectManager.h"
#include "camera.h"
#include "MeshRenderer.h"
#include "Collider.h"
#include "RigidBody.h"
#include <map>

void ObjectManager::Initialize()
{
    m_GameObjects.clear();
}

void ObjectManager::Finalize()
{
    m_GameObjects.clear();
}

void ObjectManager::PhysicsUpdate(double elapsedTime)
{
    // シミュレーションを進める
    m_PhysicsSystem.PhysicsUpdate(elapsedTime);

    // 衝突を更新
    m_PhysicsSystem.UpdateCollisions();

    // RigidBodyを取得
    auto rigidbodies = GetComponents<RigidBody>();

    // 剛体を更新
    m_PhysicsSystem.UpdateRigidBody(rigidbodies);

    // レイを取得
    auto raycasts = GetGameObjects<RayCast>();

    // レイを取得
    m_PhysicsSystem.UpdateRayCasts(raycasts);
}

void ObjectManager::PreUpdate(double elapsedTime)
{
    for (const auto& obj : m_GameObjects) {
        if (obj->m_IsActive) obj->PreUpdate(elapsedTime);
    }
}

void ObjectManager::Update(double elapsedTime)
{
    for (const auto& obj : m_GameObjects) {
        if(obj->m_IsActive) obj->Update(elapsedTime);
    }
}

void ObjectManager::PostUpdate(double elapsedTime)
{
    for (const auto& obj : m_GameObjects) {
        if (obj->m_IsActive) obj->PostUpdate(elapsedTime);
    }
    DestroyGameObjects();
    AddPendingGameObjects();
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
	obj->m_Owner = this;

    // 保留リストに追加
    m_PendingGameObjects.push_back(std::move(obj));
}

void ObjectManager::RegisterComponent(Component* component)
{
    if (!component) return;

    // type_index でラップ
    const std::type_index type(typeid(*component));
	m_ComponentMap[type].push_back(component);

    // ColliderかRigidBodyの場合はPhysicsSystemに登録
    if (type == typeid(Collider))
        m_PhysicsSystem.RegisterColliders(static_cast<Collider*>(component));
    else if (type == typeid(RigidBody))
        m_PhysicsSystem.RegisterRigidBodies(static_cast<RigidBody*>(component));
}

void ObjectManager::AddPendingGameObjects()
{
    for (auto* obj : m_PendingGameObjects)
    {
        // オブジェクトを型ごとのマップに登録
        const std::type_index type(typeid(*obj));  // type_index でラップ

        obj->m_ID = m_GameObjectMap[type].size();
        m_GameObjectMap.at(type).push_back(m_GameObjects.size());

        // オブジェクトをリストに追加
        m_GameObjects.push_back(std::unique_ptr<GameObject>(obj));
    }
}

void ObjectManager::DestroyGameObjects()
{
    for (auto& obj : m_GameObjects)
    {
        if (obj->CanDestroy())
        {
            uint64_t id = obj->m_ID;
            const std::type_index type(typeid(*obj));  // type_index でラップ
            auto& erace = m_GameObjectMap.at(type).at(id);
            auto& last = m_GameObjectMap.at(type).back();

            // 更新用のvectorから削除
            std::swap(m_GameObjects.at(erace), m_GameObjects.at(last));
            m_GameObjects.pop_back();

            // 型別マップから削除
            std::swap(erace, last);
            m_GameObjectMap.at(type).pop_back();

            // 移動したオブジェクトのIDを更新
            m_GameObjects.at(last)->m_ID = id;
        }
    }
}
