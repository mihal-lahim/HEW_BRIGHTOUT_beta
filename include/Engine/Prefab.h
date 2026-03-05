
#ifndef PREFAB_H
#define PREFAB_H

class GameObject;


class Prefab
{
public:
	Prefab() = default;
	virtual ~Prefab() = default;
	// インスタンス化メソッド
	virtual void Instantiate(GameObject& gameObject) = 0;
};



#endif
