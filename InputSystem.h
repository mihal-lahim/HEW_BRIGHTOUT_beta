
#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

class EngineCore;
class Scene;

class InputSystem
{
private:
	// 所属するエンジンコア
	EngineCore* m_engineCore = nullptr;
public:
	InputSystem(EngineCore* engineCore)
		: m_engineCore(engineCore)
	{ Initialize(); }
	~InputSystem() { Finalize(); }
	// 入力システムの初期化
	void Initialize();
	// 入力システムの更新
	void Update(const Scene& scene);
	// 入力システムの終了処理
	void Finalize();
};


#endif
