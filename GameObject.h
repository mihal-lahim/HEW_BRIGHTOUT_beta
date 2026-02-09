#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <type_traits>
#include <vector>
#include "Transform.h"
#include "Object.h"
#include "GameContext.h"
#include <string>

class Scene;
class Component;


// ゲーム内のすべてのオブジェクトの基底クラス
class GameObject final : public Object
{
public:
	GameObject() = default;
	virtual ~GameObject() = default;

	// Transformコンポーネントの取得メソッド
	Transform& transform() const 
	{ 
		return *m_transform;
	}

	// オブジェクトのアクティブ状態を設定するメソッド
	void SetActive(bool active);

	// オブジェクトのアクティブ状態を取得するメッド
	bool IsActiveSelf() const 
	{ 
		return m_isActive;
	}

	// オブジェクトが階層内でアクティブかどうかを取得するメソッド
	bool IsActiveInHierarchy() const;

	// 親オブジェクトの設定メソッド
	void SetParent(GameObject& parent);

	// 子オブジェクトの設定メソッド
	void SetChild(GameObject& child);

	// 親オブジェクトの取得メソッド
	GameObject* GetParent() const;

	// 子オブジェクトの取得メソッド
	std::vector<GameObject*> GetChildren() const;

	// 親オブジェクトの削除メソッド
	void RemoveParent();

	// 子オブジェクトの削除メソッド
	void RemoveChild(GameObject& child);

	// 名前で子オブジェクトを再帰的に検索するメソッド
	GameObject* FindChildByName(const std::string& name) const;

	// オブジェクトの破壊を許可するメソッド
	void Destroy();

	// タグの設定メソッド
	void SetTag(const std::string& tag) 
	{ 
		m_tag = tag;
	}

	// タグの比較メソッド
	bool CompareTag(const std::string& tag) const 
	{ 
		return m_tag == tag;
	}

	// オブジェクト名の設定メソッド
	void SetName(const std::string& name) 
	{ 
		m_name = name; 
	}

	// オブジェクト名の取得メソッド
	const std::string& GetName() const 
	{ 
		return m_name; 
	}

	// ゲームオブジェクトを生成するメソッド
	GameObject* CreateGameObject();

	// プレハブからゲームオブジェクトを生成するメソッド
	GameObject* Instantiate(Prefab& prefab);

	// システムを取得するメソッド
	SceneSystem& scene() const;

	// ウィンドウシステムを取得するメソッド
	WindowSystem& window() const;

	// 物理システムを取得するメソッド
	PhysicsSystem& physics() const;

	// 入力システムを取得するメソッド
	InputSystem& input() const;

	// レンダリングシステムを取得するメソッド
	RenderingSystem& rendering() const;

	// リソースシステムを取得するメソッド
	ResourceSystem& resource() const;

	// 所持しているコンポーネントの取得テンプレートメソッド
	template<typename T>
		requires std::is_base_of<Component, T>::value
	T* GetComponent() const;

	// 所持しているコンポーネントの配列取得テンプレートメソッド
	template<typename T>
		requires std::is_base_of<Component, T>::value
	std::vector<T*> GetComponents() const;


	// コンポーネント追加テンプレートメソッド
	template<typename T, typename... Args>
		requires std::is_base_of<Component, T>::value
	T* AddComponent(Args&&... args);

private:
	// 現在のゲームコンテキスト
	GameContext m_gameContext{};

	// 所属しているシーン
	Scene* m_scene = nullptr;

	// 位置・回転・スケール情報
	Transform* m_transform = nullptr;

	// オブジェクトのアクティブ状態
	bool m_isActive = true;

	// オブジェクトの破壊状態
	bool m_isDestroyed = false;

	// タグ情報
	std::string m_tag = "None";

	// オブジェクト名
	std::string m_name = "GameObject";

	// 所持しているコンポーネントの配列
	std::vector<Component*> m_components;

	friend class Scene;
	friend class Component;
};

#include "GameObject.inl"

#endif