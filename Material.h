#ifndef MATERIAL_H
#define MATERIAL_H

#include "Shader.h"
#include "Resource.h"
#include <memory>
#include <type_traits>
#include "Texture.h"

// シェーダーの組み合わせテンプレートクラス
template<typename VS, typename PS>
	requires std::is_base_of_v<VertexShader, VS>&& std::is_base_of_v<PixelShader, PS>
class ShaderT
{
public:
	// シェーダー本体
	VS* vs = nullptr;
	PS* ps = nullptr;

	// バインド
	void Bind(GraphicsDevice& device);
};


// 基底マテリアルクラス
class Material : public Resource
{
public:
	virtual ~Material() = default;
};


// マテリアルの内部実装
template<typename VS, typename PS>
	requires std::is_base_of_v<VertexShader, VS>&& std::is_base_of_v<PixelShader, PS>
class MaterialImpl : public Material
{
public:
	// マテリアル用定数バッファの型定義
	using CBType = typename PS::PerMaterialCB;

	// マテリアルデータ
	CBType property = {};

	// テクスチャ
	Texture* texture = nullptr;

	// バッファの作成、更新、バインド
	bool CreateBuffer(GraphicsDevice& device);
	void UpdateBuffer(GraphicsDevice& device);
	void Bind(GraphicsDevice& device);

private:
	// シェーダーの組み合わせ
	ShaderT<VS, PS> shader = {};
	// 定数バッファ
	ConstantBuffer<CBType>* cb = nullptr;
};

// 2D用マテリアルと3D用マテリアルのエイリアス
using Material2D = MaterialImpl<SpriteVS, SpritePS>;
using Material3D = MaterialImpl<MeshVS, MeshPS>;



#include "Material.inl"

#endif
