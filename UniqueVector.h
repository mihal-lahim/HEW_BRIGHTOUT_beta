
#ifndef UNIQUE_VECTOR_H
#define UNIQUE_VECTOR_H

#include <vector>
#include <unordered_set>


template<typename T>
class UniqueVector
{
public:
	UniqueVector() = default;
	~UniqueVector() = default;

	// 要素取得メソッド
	T& at(size_t index)
	{
		return m_vector.at(index);
	}

	// サイズ取得メソッド
	size_t size() const
	{
		return m_vector.size();
	}

	// 要素の追加メソッド
	void push_back(const T& value)
	{
		if (m_set.find(value) == m_set.end())
		{
			m_vector.push_back(value);
			m_set.insert(value);
		}
	}

	// 要素の削除メソッド
	void pop_back()
	{
		if (!m_vector.empty())
		{
			T back = m_vector.back();
			m_vector.pop_back();
			m_set.erase(back);
		}
	}

	// 空かどうかの判定メソッド
	bool empty() const
	{
		return m_vector.empty();
	}

	// リセットメソッド
	void clear()
	{
		m_vector.clear();
		m_set.clear();
	}

private:
	std::vector<T> m_vector = {};
	std::unordered_set<T> m_set = {};
};


#endif
