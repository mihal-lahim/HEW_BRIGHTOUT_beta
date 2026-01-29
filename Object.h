
#ifndef OBJECT_H
#define OBJECT_H

#include <typeindex>
#include <cstdint>

template<typename T>
class ObjectPool;

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
	friend class ObjectPool;
};

#endif
