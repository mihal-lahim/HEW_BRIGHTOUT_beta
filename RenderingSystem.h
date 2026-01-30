
#ifndef RENDERING_SYSTEM_H
#define RENDERING_SYSTEM_H


class Camera;
class MeshRenderer;
class EngineCore;

class RenderingSystem
{
private:
	// 所属するエンジンコア
	EngineCore* m_engineCore = nullptr;

public:

	RenderingSystem(EngineCore* engineCore)
		: m_engineCore(engineCore)
	{ Initialize(); }
	~RenderingSystem() { Finalize(); }

	void Initialize();
	void Finalize();

	// 描画メソッド
	void Render(const Scene& scene);
};


#endif
