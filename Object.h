
#ifndef OBJECT_H
#define OBJECT_H

#include <typeindex>
#include <cstdint>


class Object
{
private:
	// メモリ確保ID
	uint32_t m_allocationID = 0;
public:
	// コンストラクタ
	Object() = default;
	virtual ~Object() = default;

	// オブジェクトの型を比較するテンプレートメソッド
	template<typename T>
	bool CompareType() { return typeid(*this) == typeid(T); }

	template<typename T>
		requires std::is_base_of<Object, T>::value
	friend class ObjectPool;

	friend class Scene;
};

#endif
