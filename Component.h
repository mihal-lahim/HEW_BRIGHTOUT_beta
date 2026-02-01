#ifndef COMPONENT_H
#define COMPONENT_H


#include "Object.h"

class ObjectManager;
class GameObject;
class PhysicsSystem;
class SceneSystem;
class InputSystem;
class Prefab;

class Component : public Object
{
public:
	// コンストラクタ・デストラクタ
	Component() = default;
	virtual ~Component() = default;

	// 所有しているゲームオブジェクトの取得メソッド
	GameObject& gameObject() const { return *m_gameObject; }

	// オブジェクトのアクティブ状態を取得するメソッド
	bool IsEnable() const { return m_isEnable; }

	// オブジェクトのアクティブ状態を設定するメソッド
	void SetEnable(bool enable);

	// 有効化・無効化時の仮想メソッド
	virtual void OnEnable() {};
	virtual void OnDisable() {};

	// オブジェクトの破壊を許可するメソッド
	void Destroy();

	// ゲームオブジェクトを生成するメソッド
	GameObject* CreateGameObject();

	// プレファブからゲームオブジェクトを生成するテンプレートメソッド
	GameObject* Instantiate(const Prefab& prefab);

	// コンポーネントの型IDを取得するテンプレートメソッド
	template<typename T>
		requires std::is_base_of<Component, T>::value
	static uint32_t GetTypeID();

	// 物理システムの取得メソッド
	PhysicsSystem& physics();

	// シーンシステムの取得メソッド
	SceneSystem& scene();

	// 入力システムの取得メソッド
	InputSystem& input();


	// ゲームコンテキストを使った初期化メソッド
	virtual void InitializeByContext() {};
	// ゲームコンテキストを使った終了メソッド
	virtual void FinalizeByContext() {};


private:
	// 所有しているゲームオブジェクト
	GameObject* m_gameObject = nullptr;

	// オブジェクトのアクティブ状態
	bool m_isEnable = true;

	// コンポーネント型ID
	uint32_t m_typeID = 0;

	// コンポーネント型ID生成用カウンタ
	static inline uint32_t m_counter = 0;


	friend class GameObject;
	friend class Scene;
};

#include "Component.inl"


#endif
