
#ifndef UNIQUE_QUEUE_H
#define UNIQUE_QUEUE_H

#include <queue>
#include <unordered_set>

template<typename T>
class UniqueQueue
{
public:
	// 要素の追加メソッド
	void push(const T& value)
	{
		// 要素がセットに存在しない場合のみ追加
		if (m_set.find(value) == m_set.end())
		{
			m_queue.push(value);
			m_set.insert(value);
		}
	}

	// 要素の削除メソッド
	void pop()
	{
		if (!m_queue.empty())
		{
			T front = m_queue.front();
			m_queue.pop();
			m_set.erase(front);
		}
	}

	// 先頭要素取得メソッド
	T& front() { return m_queue.front(); }

	// 空かどうかの判定メソッド
	bool empty() const { return m_queue.empty(); }

	// リセットメソッド
	void clear()
	{
		while (!m_queue.empty())
		{
			m_queue.pop();
		}
		m_set.clear();
	}

private:
	// 内部キュー
	std::queue<T> m_queue;
	// 内部セット
	std::unordered_set<T> m_set;
};


#endif
