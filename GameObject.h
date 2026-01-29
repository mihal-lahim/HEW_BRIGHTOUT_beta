#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <type_traits>
#include <vector>
#include "Transform.h"
#include "Object.h"

class GameContext;
class Component;


// ゲーム内のすべてのオブジェクトの基底クラス
class GameObject : public Object
{
private:
	// 現在のゲームコンテキスト
	GameContext* m_gameContext;

	// 位置・回転・スケール情報
	Transform* m_transform = nullptr;

	// オブジェクトのアクティブ状態
	bool m_isActive = true;

	// 破壊可能フラグ
	bool m_canDestroy = false;

	// 所持しているコンポーネントの配列
	std::vector<Component*> m_components;

	// ゲームオブジェクト型ID生成用カウンタ
	static inline size_t m_counter = 0;
public:

	GameObject() = default;
    virtual ~GameObject() = default;

	// Transformコンポーネントの取得メソッド
	Transform* const transform() const { return m_transform; }

	// オブジェクトのアクティブ状態を設定するメソッド
	void SetActive(bool active) { m_isActive = active; }

	// オブジェクトのアクティブ状態を取得するメッド
	bool IsActiveSelf() const { return m_isActive; }

	// オブジェクトの破壊を許可するメソッド
	void Destroy() { m_canDestroy = true; }


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
	T* AddComponent(Args... args);


	// ゲームオブジェクト作成テンプレートメソッド
	template<typename T, typename... Args>
	T* CreateGameObject(Args... args);


	// ゲームオブジェクトの型ID取得テンプレートメソッド
	template<typename T>
		requires std::is_base_of<GameObject, T>::value
	static size_t GetTypeID();
};

#endif