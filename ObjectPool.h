
#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <vector>
#include <memory>
#include <type_traits>
#include "Object.h"

// オブジェクトプールの基底クラス
class ObjectPoolBase
{
public:
	virtual ~ObjectPoolBase() = default;
};


// 特定の型Tのオブジェクトを効率的に管理・再利用するためのオブジェクトプールクラス
template<typename T>
	requires std::is_base_of<Object, T>::value
class ObjectPool : public ObjectPoolBase
{
private:
	// オブジェクトのプール
	std::vector<std::unique_ptr<T>> m_pool;
public:

	// 既存のオブジェクトをプールに登録
	void Register(T* obj)
	{
		// オブジェクトのallocationIDを設定
		obj->m_allocationID = static_cast<uint32_t>(m_pool.size());
		m_pool.push_back(std::unique_ptr<T>(obj));
	}


	// プールからオブジェクトを取得
	T* Get(uint32_t idx)
	{
		if (m_pool.size() <= idx) return nullptr;
		return m_pool.at(idx).get();
	}

	// プール内のオブジェクト数を取得
	size_t Size() const
	{
		return m_pool.size();
	}

	// オブジェクトを削除
	void Destroy(uint32_t idx)
	{
		if (m_pool.size() <= idx) return;

		// allocationID の安定性を優先して、要素を詰めずに破棄
		m_pool.at(idx).reset();
	}
};








#endif
