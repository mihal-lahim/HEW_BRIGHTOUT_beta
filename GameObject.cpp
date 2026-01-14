#include "GameObject.h"
#include "model.h"
#include "cube.h" // モデルがない場合の描画用

using namespace DirectX;


void GameObject::DestroyComponents()
{
	// コンポーネントリストを走査して破棄可能なコンポーネントを削除
	for (auto& component : m_Components) {
        if (!component->m_CanDestroy) continue;

		// 削除するコンポーネントのIDを取得
		uint64_t compID = component->m_ID;
        
		// コンポーネントマップを更新
        std::type_index typeIndex = std::type_index(typeid(m_Components.at(compID)));
		m_ComponentMap.erase(typeIndex);

		// 最後尾のコンポーネントと入れ替え
		std::swap(component, m_Components.back());

        // 削除
		m_Components.pop_back();

		// コンポーネントIDの更新
		m_Components.at(compID)->m_ID = compID;
    }
}

void GameObject::PreUpdate(double elapsedTime)
{
	// まだStartされていないコンポーネントのStartを呼び出す
    for (auto& prestart : m_PreStarted) {
		prestart->Start();
    }

    for(auto& component : m_Components) {
        if(component->m_IsActive) {
            component->PreUpdate(elapsedTime);
        }
	}
}

void GameObject::Update(double elapsedTime)
{
    for (auto& component : m_Components) {
        if (component->m_IsActive) {
            component->Update(elapsedTime);
        }
    }
}

void GameObject::PostUpdate(double elapsedTime)
{
    for (auto& component : m_Components) {
        if (component->m_IsActive) {
            component->PostUpdate(elapsedTime);
        }
    }
}
