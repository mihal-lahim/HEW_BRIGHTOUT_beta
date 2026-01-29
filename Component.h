#ifndef COMPONENT_H
#define COMPONENT_H


#include "Object.h"

class ObjectManager;
class GameObject;
class GameContext;


class Component : public Object
{
private:
	// 所有しているゲームオブジェクト
	GameObject* m_gameObject = nullptr;

	// 現在のゲームコンテキスト
	GameContext* m_gameContext = nullptr;

	// オブジェクトのアクティブ状態
	bool m_isActive = true;

	// 破壊可能フラグ
	bool m_canDestroy = false;

	// コンポーネント型ID生成用カウンタ
	static inline size_t m_counter = 0;
public:

	// コンストラクタ・デストラクタ
	Component() = default;
	virtual ~Component() = default;

	// 所有しているゲームオブジェクトの取得メソッド
	GameObject* const GetGameObject() const { return m_gameObject; }

	// オブジェクトのアクティブ状態を取得するメソッド
	bool IsActive() const;

	// オブジェクトのアクティブ状態を設定するメソッド
	virtual void SetActive(bool active);

	// オブジェクトの破壊を許可するメソッド
	void Destroy() { m_canDestroy = true; }

	// コンポーネントの型IDを取得するテンプレートメソッド
	template<typename T>
		requires std::is_base_of<Component, T>::value
	static size_t GetTypeID();

	friend class GameObject;
};


#endif
