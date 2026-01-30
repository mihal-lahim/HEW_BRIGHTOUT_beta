#ifndef COMPONENT_H
#define COMPONENT_H


#include "Object.h"

class ObjectManager;
class GameObject;
struct GameContext;


class Component : public Object
{
private:
	// 現在のゲームコンテキスト
	GameContext* m_gameContext;

	// 所有しているゲームオブジェクト
	GameObject* m_gameObject = nullptr;

	// オブジェクトのアクティブ状態
	bool m_isActive = true;

	// コンポーネント型ID生成用カウンタ
	static inline size_t m_counter = 0;
public:

	// コンストラクタ・デストラクタ
	Component() = default;
	virtual ~Component() = default;

	// 所有しているゲームオブジェクトの取得メソッド
	GameObject& gameObject() const { return *m_gameObject; }

	// オブジェクトのアクティブ状態を取得するメソッド
	bool IsActive() const;

	// オブジェクトのアクティブ状態を設定するメソッド
	virtual void SetActive(bool active);

	// オブジェクトの破壊を許可するメソッド
	void Destroy();


	// ゲームコンテキストを使った初期化メソッド
	virtual void ContextInitialize() {};


	// ゲームオブジェクト作成テンプレートメソッド
	template<typename T, typename... Args>
	T* Instantiate(Args... args);

	// コンポーネントの型IDを取得するテンプレートメソッド
	template<typename T>
		requires std::is_base_of<Component, T>::value
	static uint32_t GetTypeID();


	friend class GameObject;
	friend class Scene;
};


#endif
