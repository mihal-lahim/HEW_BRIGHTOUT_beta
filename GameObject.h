#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <DirectXMath.h>
#include "collision.h"
#include <cstdint>
#include <string>

struct MODEL; // モデル構造体の前方宣言
class ObjectManager;


// ゲーム内のすべての静的オブジェクトの基底クラス
class GameObject
{
private:
    uint64_t m_ID = 0; // オブジェクトの型ごとの一意なID
    bool m_CanDestroy = false; // オブジェクトが破壊可能かどうか
	ObjectManager* m_owner = nullptr; // 所有者の ObjectManager へのポインタ
	bool m_IsActive = true; // オブジェクトがアクティブかどうか
protected:
    DirectX::XMFLOAT3 m_Position{};
    DirectX::XMFLOAT3 m_Scale{ 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT3 m_Rotation{}; // オイラー角
    MODEL* m_pModel = nullptr;
    int m_TextureId = -1; // テクスチャID（モデルがない場合）
public:
    GameObject(const DirectX::XMFLOAT3& pos, MODEL* model = nullptr, int textureId = -1, const DirectX::XMFLOAT3& scale = { 1.0f, 1.0f, 1.0f })
        : m_Position(pos), m_pModel(model), m_TextureId(textureId), m_Scale(scale) {}
    virtual ~GameObject() = default;


	// 各種更新メソッド　overrideして使用
    virtual void Start() {}
	virtual void PreUpdate(double) {}
    virtual void Update(double) {}
	virtual void PostUpdate(double) {}
	virtual void Draw() const;

    virtual AABB GetAABB() const;

	// 位置、回転、スケールの取得・設定メソッド
    const DirectX::XMFLOAT3& GetPosition() const { return m_Position; }
    const DirectX::XMFLOAT3& GetRotation() const { return m_Rotation; }
	const DirectX::XMFLOAT3& GetScale() const { return m_Scale; }

	// 位置、回転、スケールの設定メソッド
	void SetPosition(const DirectX::XMFLOAT3& position) { m_Position = position; }
    void SetRotation(const DirectX::XMFLOAT3& rotation) { m_Rotation = rotation; }
	void SetScale(const DirectX::XMFLOAT3& scale) { m_Scale = scale; }


	// アクティブフラグの取得・設定メソッド
	bool IsActive() const { return m_IsActive; }
	void SetActive(bool isActive) { m_IsActive = isActive; }


	// Owner ObjectManager の取得・設定メソッド
	ObjectManager* GetOwner() const { return m_owner; }


	// 破壊可能フラグの取得・設定メソッド
	bool CanDestroy() const { return m_CanDestroy; }
	void Destroy() { m_CanDestroy = true; }


	friend class ObjectManager; // ObjectManager クラスをフレンドに指定
};

#endif // GAME_OBJECT_H
