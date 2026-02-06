#ifndef SHADER_H
#define SHADER_H

#include "Resource.h"
#include "GraphicsDevice.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <d3d11.h>
#include <wrl/client.h>

// 基底シェーダークラス
class Shader : public Resource
{
};

// シェーダー数値タイプ列挙型
enum class ShaderValueType
{
	None,
	Float,
	Float4,
	Matrix4x4,
};

// 定数バッファの変数情報構造体
struct ConstantBufferVariableInfo
{
	std::string name;
	size_t offset = 0;
	size_t size = 0;
	ShaderValueType type = ShaderValueType::None;
};

// 定数バッファ情報構造体
struct ConstantBufferInfo
{
	size_t size = 0;
	std::unordered_map<std::string, ConstantBufferVariableInfo> variables;
};

// シェーダーリフレクション情報構造体
struct ShaderReflectionInfo
{
	std::unordered_map<std::string, ConstantBufferInfo> buffers;
	std::unordered_map<std::string, ConstantBufferVariableInfo> constantBuffers;
};


// 入力要素情報構造体
struct InputElementInfo
{
	std::string name;
	UINT index;
	DXGI_FORMAT format;
	UINT offset;
};

// シェーダー入力レイアウト情報構造体
struct ShaderInputLayoutInfo
{
	std::vector<InputElementInfo> elements;
};


// 頂点シェーダークラス
class VertexShader : public Shader
{
public:
	// バッファの作成、バインド
	bool CreateBuffer(GraphicsDevice& device, const std::string& filePath);
	void Bind(GraphicsDevice& device);

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vs = nullptr;
	ShaderReflectionInfo reflectionInfo = {};
	ShaderInputLayoutInfo inputLayoutInfo = {};
};

// ピクセルシェーダークラス
class PixelShader : public Shader
{
public:
	// バッファの作成、バインド
	bool CreateBuffer(GraphicsDevice& device, const std::string& filePath);
	void Bind(GraphicsDevice& device);

	Microsoft::WRL::ComPtr<ID3D11PixelShader> ps = nullptr;
	ShaderReflectionInfo reflectionInfo = {};
};


// シェーダープログラムクラス
class ShaderProgram : public Resource
{
public:
	// バッファの作成、バインド
	bool CreateBuffer(GraphicsDevice& device, const std::string& vsFilePath, const std::string& psFilePath);
	void Bind(GraphicsDevice& device);

	VertexShader* vertexShader = nullptr;
	PixelShader* pixelShader = nullptr;

	ShaderReflectionInfo mergedReflectionInfo = {};
	ShaderInputLayoutInfo inputLayoutInfo = {};
};


#endif
