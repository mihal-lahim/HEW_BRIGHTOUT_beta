#ifndef CONTROLLER_MANAGER_H
#define CONTROLLER_MANAGER_H

class Controller;

/**
 * @brief シングルトンパターンのコントローラーマネージャー
 * 全シーンからコントローラーにアクセス可能
 */
class ControllerManager
{
public:
	// シングルトンインスタンス取得
	static ControllerManager& GetInstance();

	// 初期化・終了処理
	void Initialize();
	void Finalize();
	void Update();

	// コントローラーアクセス
	Controller* GetController(int index) const;

private:
	// シングルトン：プライベートコンストラクタ
	ControllerManager();
	~ControllerManager();

	// コピー・ムーブ禁止
	ControllerManager(const ControllerManager&) = delete;
	ControllerManager& operator=(const ControllerManager&) = delete;
	ControllerManager(ControllerManager&&) = delete;
	ControllerManager& operator=(ControllerManager&&) = delete;

	// メンバ変数
	Controller* m_controllers[3];
	bool m_initialized;
};

#endif // CONTROLLER_MANAGER_H
