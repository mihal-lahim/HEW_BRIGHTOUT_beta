
#ifndef OBJECT_H
#define OBJECT_H

#include <typeindex>
#include <cstdint>
#include <type_traits>
#include "GameManager.h"

class ObjectManager;

using ObjectID = uint32_t;

class Object
{
private:
	// オブジェクトの所有者オブジェクトマネージャー
	ObjectManager* m_ObjectManager = nullptr;

	// オブジェクトの一意なID
	ObjectID m_ID = 0;

	// オブジェクトが破壊可能かどうか
	bool m_CanDestroy = false;

	// オブジェクトがアクティブかどうか
	bool m_IsActive = true;

	// オブジェクトがStart済みかどうか
	bool m_HasStarted = false;
public:

	// コンストラクタ
	Object() = default;
	virtual ~Object() = default;


	// オブジェクトを破壊可能にするメソッド
	void Destroy() { m_CanDestroy = true; }

	// オブジェクトが破壊可能かどうかを取得するメソッド
	bool CanDestroy() const { return m_CanDestroy; }



	// オブジェクトのアクティブ状態を設定するメソッド
	virtual void SetActive(bool active) { m_IsActive = active; }

	// オブジェクトのアクティブ状態を取得するメソッド
	virtual bool IsActive() const { return m_IsActive; }


	// オブジェクトの所有者オブジェクトマネージャーを取得するメソッド
	ObjectManager* const ObjectManager() const { return m_ObjectManager; }


	// オブジェクトの型を比較するテンプレートメソッド
	template<typename T>
	void CompareType() { return typeid(this*) == typeid(T); }



	// ライフサイクルメソッド
	// Awake: 作成時に一度だけ呼ばれる
	virtual void Awake() {};

	// Start: 最初にPreUpdate()される前に一度だけ呼ばれる
	virtual void Start() {};

	// 各種更新メソッド
	virtual void PreUpdate() {};
	virtual void Update() {};
	virtual void PostUpdate() {};


	friend class ObjectManager;
};



// Object を継承している型に制約をかけるコンセプト
template<typename T>
concept ObjectDerived = std::is_base_of<Object, T>::value;


#endif
