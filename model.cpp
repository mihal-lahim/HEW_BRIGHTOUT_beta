#include "GraphicsDevice.h"
using namespace DirectX;
#include "WICTextureLoader11.h"
#include "DirectXTex.h"
#include "shader3d.h"
#include "texture.h"
#include "Mesh.h"
#include <cassert>
#include <string>
#include "model.h"
#include <windows.h>
#include <cstdarg>
#include <wrl/client.h>

static unsigned int g_WhiteTexId;

static ID3D11Device* g_pDevice = nullptr;
static ID3D11DeviceContext* g_pContext = nullptr;
static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> s_fallbackWhiteSRV = nullptr;

// デバッグ出力
static void DBG(const char* fmt, ...)
{
	char buf[1024];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf_s(buf, _countof(buf), _TRUNCATE, fmt, ap);
	va_end(ap);
	OutputDebugStringA(buf);
	OutputDebugStringA("\n");
}

void ModelInitialize(GraphicsDevice* device)
{
	g_pDevice = device->GetDevice();
	g_pContext = device->GetDeviceContext();
}

//MODEL* ModelLoad(const char* FileName, float scale)
//{
//	MODEL* model = new MODEL;
//
//
//	const std::string modelPath(FileName);
//
//	model->AiScene = aiImportFile(FileName, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
//	assert(model->AiScene);
//
//	model->VertexBuffer = new ID3D11Buffer * [model->AiScene->mNumMeshes];
//	model->IndexBuffer = new ID3D11Buffer * [model->AiScene->mNumMeshes];
//
//
//	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
//	{
//		aiMesh* mesh = model->AiScene->mMeshes[m];
//
//		// 頂点バッファ生成
//		{
//			Vertex3d* vertex = new Vertex3d[mesh->mNumVertices]{};
//
//			for (unsigned int v = 0; v < mesh->mNumVertices; v++)
//			{
//
//				vertex[v].position = XMFLOAT3(mesh->mVertices[v].x * scale, mesh->mVertices[v].y * scale, mesh->mVertices[v].z * scale);
//				vertex[v].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
//
//				// 法線が存在するか確認
//				if (mesh->mNormals)
//				{
//					// 元コードの軸入れ替えはそのまま維持
//					vertex[v].normal = XMFLOAT3(mesh->mNormals[v].x, -mesh->mNormals[v].z, mesh->mNormals[v].y);
//				}
//				else
//				{
//					vertex[v].normal = XMFLOAT3(0, 1, 0);
//				}
//
//				// UV があるかチェック
//				if (mesh->mTextureCoords && mesh->mTextureCoords[0])
//				{
//					vertex[v].texcoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
//				}
//				else
//				{
//					vertex[v].texcoord = XMFLOAT2(0.0f, 0.0f);
//				}
//			}
//
//			D3D11_BUFFER_DESC bd{};
//			bd.Usage = D3D11_USAGE_DEFAULT;
//			bd.ByteWidth = sizeof(Vertex3d) * mesh->mNumVertices;
//			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//			bd.CPUAccessFlags = 0;
//
//			D3D11_SUBRESOURCE_DATA sd{};
//			sd.pSysMem = vertex;
//
//			g_pDevice->CreateBuffer(&bd, &sd, &model->VertexBuffer[m]);
//
//
//
//			delete[] vertex;
//		}
//
//
//		// インデックスバッファ生成
//		{
//			unsigned int* index = new unsigned int[mesh->mNumFaces * 3];
//
//			for (unsigned int f = 0; f < mesh->mNumFaces; f++)
//			{
//				const aiFace* face = &mesh->mFaces[f];
//
//				assert(face->mNumIndices == 3);
//
//				index[f * 3 + 0] = face->mIndices[0];
//				index[f * 3 + 1] = face->mIndices[1];
//				index[f * 3 + 2] = face->mIndices[2];
//			}
//
//			D3D11_BUFFER_DESC bd{};
//			bd.Usage = D3D11_USAGE_DEFAULT;
//			bd.ByteWidth = sizeof(unsigned int) * mesh->mNumFaces * 3;
//			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//			bd.CPUAccessFlags = 0;
//
//			D3D11_SUBRESOURCE_DATA sd{};
//			sd.pSysMem = index;
//
//			g_pDevice->CreateBuffer(&bd, &sd, &model->IndexBuffer[m]);
//
//
//
//			delete[] index;
//		}
//	}
//
//
//
//	// 埋め込みテクスチャ読み込み（scene->mTextures）
//	for (unsigned int i = 0; i < model->AiScene->mNumTextures; i++)
//	{
//		aiTexture* aitexture = model->AiScene->mTextures[i];
//
//
//		ID3D11ShaderResourceView* texture = nullptr;
//		// 圧縮済みデータでは aitexture->mHeight == 0 なので注意（ここでは WIC 経由で扱う例）
//		if (aitexture->mHeight != 0 && aitexture->pcData)
//		{
//			TexMetadata metadata;
//			ScratchImage image;
//			HRESULT hr = LoadFromWICMemory((const void*)aitexture->pcData, aitexture->mWidth, WIC_FLAGS_NONE, &metadata, image);
//			if (SUCCEEDED(hr))
//			{
//				CreateShaderResourceView(g_pDevice, image.GetImages(), image.GetImageCount(), metadata, &texture);
//			}
//		}
//		// キーはファイル名があるならファイル名、それと "*i" を両方登録しておく
//		std::string keyFile = aitexture->mFilename.length ? aitexture->mFilename.data : std::string();
//		if (texture)
//		{
//			if (!keyFile.empty()) model->Texture[keyFile] = texture;
//			// "*N" 形式の参照にも対応
//			char idxKey[16];
//			sprintf_s(idxKey, "*%u", i);
//			model->Texture[std::string(idxKey)] = texture;
//		}
//	}
//
//
//	// 白テクスチャはフォールバックとしてロードしておく
//	g_WhiteTexId = Texture_Load(L"texture/white.png"); // サーフェスカラー用
//
//	return model;
//}

MODEL* ModelLoad(const char* FileName, float scale)
{
	DBG("ModelLoad: FileName=\"%s\" scale=%f", FileName ? FileName : "(null)", scale);
	MODEL* model = new MODEL;

	const std::string modelPath(FileName ? FileName : "");

	model->AiScene = aiImportFile(FileName, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);
	if (!model->AiScene) {
		delete model;
		return nullptr;
	}

	model->VertexBuffer = new ID3D11Buffer * [model->AiScene->mNumMeshes];
	model->IndexBuffer = new ID3D11Buffer * [model->AiScene->mNumMeshes];

	// nullptr 初期化
	for (unsigned int i = 0; i < model->AiScene->mNumMeshes; ++i) {
		model->VertexBuffer[i] = nullptr;
		model->IndexBuffer[i] = nullptr;
	}

	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = model->AiScene->mMeshes[m];

		// 頂点バッファ生成
		{
			Vertex3d* vertex = new Vertex3d[mesh->mNumVertices]{};

			for (unsigned int v = 0; v < mesh->mNumVertices; v++)
			{
				vertex[v].position = XMFLOAT3(mesh->mVertices[v].x * scale, mesh->mVertices[v].y * scale, mesh->mVertices[v].z * scale);
				vertex[v].color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

				if (mesh->mNormals)
				{
					vertex[v].normal = XMFLOAT3(mesh->mNormals[v].x, -mesh->mNormals[v].z, mesh->mNormals[v].y);
				}
				else
				{
					vertex[v].normal = XMFLOAT3(0, 1, 0);
				}

				if (mesh->mTextureCoords && mesh->mTextureCoords[0])
				{
					vertex[v].texcoord = XMFLOAT2(mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y);
				}
				else
				{
					vertex[v].texcoord = XMFLOAT2(0.0f, 0.0f);
				}
			}

			D3D11_BUFFER_DESC bd{};
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(Vertex3d) * mesh->mNumVertices;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd{};
			sd.pSysMem = vertex;

			HRESULT hr = g_pDevice->CreateBuffer(&bd, &sd, &model->VertexBuffer[m]);
			if (FAILED(hr)) {
				DBG("CreateBuffer(vertex) FAILED mesh=%u hr=0x%08X", m, hr);
			}

			delete[] vertex;
		}

		// インデックスバッファ生成
		{
			unsigned int* index = new unsigned int[mesh->mNumFaces * 3];

			for (unsigned int f = 0; f < mesh->mNumFaces; f++)
			{
				const aiFace* face = &mesh->mFaces[f];
				assert(face->mNumIndices == 3);
				index[f * 3 + 0] = face->mIndices[0];
				index[f * 3 + 1] = face->mIndices[1];
				index[f * 3 + 2] = face->mIndices[2];
			}

			D3D11_BUFFER_DESC bd{};
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned int) * mesh->mNumFaces * 3;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA sd{};
			sd.pSysMem = index;

			HRESULT hr = g_pDevice->CreateBuffer(&bd, &sd, &model->IndexBuffer[m]);

			delete[] index;
		}
	}

	// 埋め込みテクスチャ読み込み（scene->mTextures）
	for (unsigned int i = 0; i < model->AiScene->mNumTextures; i++)
	{
		aiTexture* aitexture = model->AiScene->mTextures[i];

		ID3D11ShaderResourceView* texture = nullptr;

		if (aitexture->pcData)
		{
			if (aitexture->mHeight != 0)
			{
				TexMetadata metadata;
				ScratchImage image;
				HRESULT hr = LoadFromWICMemory((const void*)aitexture->pcData, aitexture->mWidth, WIC_FLAGS_NONE, &metadata, image);
				if (SUCCEEDED(hr))
				{
					HRESULT hr2 = CreateShaderResourceView(g_pDevice, image.GetImages(), image.GetImageCount(), metadata, &texture);
				}
			}
			else
			{
				ID3D11Resource* resource = nullptr;
				ID3D11ShaderResourceView* srv = nullptr;
				HRESULT hr = DirectX::CreateWICTextureFromMemory(g_pDevice, g_pContext,
					(const uint8_t*)aitexture->pcData,
					(size_t)aitexture->mWidth,
					&resource,
					&srv);
				if (SUCCEEDED(hr))
				{
					texture = srv;
					if (resource) resource->Release();
				}
			}
		}

		std::string keyFile = aitexture->mFilename.length ? aitexture->mFilename.data : std::string();
		if (texture)
		{
			if (!keyFile.empty()) model->Texture[keyFile] = texture;
			char idxKey[16];
			sprintf_s(idxKey, "*%u", i);
			model->Texture[std::string(idxKey)] = texture;
		}
	}

	// 白テクスチャはフォールバックとしてロードしておく
	g_WhiteTexId = Texture_Load(L"texture/white.png"); // サーフェスカラー用
	if (g_WhiteTexId == 0 && !s_fallbackWhiteSRV)
	{
		// 白ピクセルテクスチャ作成
		UINT32 whitePixel = 0xFFFFFFFFu;
		D3D11_TEXTURE2D_DESC td{};
		td.Width = 1;
		td.Height = 1;
		td.MipLevels = 1;
		td.ArraySize = 1;
		td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		td.SampleDesc.Count = 1;
		td.Usage = D3D11_USAGE_IMMUTABLE;
		td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA init{};
		init.pSysMem = &whitePixel;
		init.SysMemPitch = sizeof(UINT32);

		ID3D11Texture2D* tex = nullptr;
		HRESULT hr = g_pDevice->CreateTexture2D(&td, &init, &tex);
		if (SUCCEEDED(hr) && tex)
		{
			ID3D11ShaderResourceView* srv = nullptr;
			hr = g_pDevice->CreateShaderResourceView(tex, nullptr, &srv);
			if (SUCCEEDED(hr))
			{
				s_fallbackWhiteSRV.Attach(srv);
			}
			tex->Release();
		}
	}

	// シーン全体の境界とマテリアル色をログ出力
	LogSceneBounds(model);
	for (unsigned int mi = 0; mi < model->AiScene->mNumMaterials; ++mi) {
		aiColor3D col(0, 0, 0);
		aiReturn r = model->AiScene->mMaterials[mi]->Get(AI_MATKEY_COLOR_DIFFUSE, col);
		(void)r;
	}

	return model;
}

void ModelRelease(MODEL* model)
{
	if (!model) return;
	if (model->AiScene) {
		for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
		{
			if (model->VertexBuffer[m]) model->VertexBuffer[m]->Release();
			if (model->IndexBuffer[m]) model->IndexBuffer[m]->Release();
		}
	}
	delete[] model->VertexBuffer;
	delete[] model->IndexBuffer;

	for (std::pair<const std::string, ID3D11ShaderResourceView*> pair : model->Texture)
	{
		if (pair.second) pair.second->Release();
	}

	if (model->AiScene) aiReleaseImport(model->AiScene);

	delete model;
}

//void ModelDraw(const MODEL* model, const DirectX::XMMATRIX& mtxWorld)
//{
//
//	// シェーダーを描画パイプラインに設定
//	Shader3d_Begin();
//
//	for (unsigned int ModelNum = 0; ModelNum < model->AiScene->mNumMeshes; ModelNum++)
//	{
//		// 頂点バッファを描画パイプラインに設定
//		UINT stride = sizeof(Vertex3d);
//		UINT offset = 0;
//		g_pContext->IASetVertexBuffers(0, 1, &model->VertexBuffer[ModelNum], &stride, &offset);
//
//
//		// 頂点インデックスを描画パイプラインに設定
//		g_pContext->IASetIndexBuffer(model->IndexBuffer[ModelNum], DXGI_FORMAT_R32_UINT, 0);
//
//
//
//		Shader3d_SetWorldMatrix(mtxWorld);
//
//
//		// プリミティブトポロジ設定
//		g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//		// テクスチャの設定
//		aiString texture;
//		aiMaterial* aimaterial = model->AiScene->mMaterials[model->AiScene->mMeshes[ModelNum]->mMaterialIndex];
//		aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture);
//
//		if (texture.length != 0)
//		{
//			//テクスチャの設定
//			g_pContext->PSSetShaderResources(0, 1, &model->Texture.at(texture.data));
//			Shader3d_SetMaterialDiffuse({ 1.0f, 1.0f, 1.0f, 1.0f });
//		}
//		else
//		{
//			Texture_SetTexture(g_WhiteTexId);
//			aiColor3D diffuse;
//			aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
//			Shader3d_SetMaterialDiffuse({ diffuse.r, diffuse.g, diffuse.b, 1.0f });
//		}
//
//		// ポリゴン描画命令発行
//		g_pContext->DrawIndexed(model->AiScene->mMeshes[ModelNum]->mNumFaces * 3, 0, 0);
//	}
//}


void ModelDraw(const MODEL* model, const DirectX::XMMATRIX& mtxWorld)
{
	(void)mtxWorld; // 未使用パラメータ警告(C4100)を抑制

	if (!model || !model->AiScene) {
		return;
	}

	for (unsigned int ModelNum = 0; ModelNum < model->AiScene->mNumMeshes; ModelNum++)
	{
		UINT stride = sizeof(Vertex3d);
		UINT offset = 0;

		ID3D11Buffer* vb = model->VertexBuffer[ModelNum];
		ID3D11Buffer* ib = model->IndexBuffer[ModelNum];
		if (!vb || !ib) {
			continue;
		}

		g_pContext->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
		g_pContext->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
		g_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		aiString texture;
		aiMaterial* aimaterial = model->AiScene->mMaterials[model->AiScene->mMeshes[ModelNum]->mMaterialIndex];
		aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texture);

		if (texture.length != 0)
		{
			auto it = model->Texture.find(texture.data);
			if (it != model->Texture.end() && it->second)
			{
				g_pContext->PSSetShaderResources(0, 1, &it->second);
				Shader3d_SetMaterialDiffuse({ 1.0f,1.0f,1.0f,1.0f });
			}
			else
			{
				DBG("Texture key \"%s\" not found in model->Texture map (maybe external file?)", texture.data);
				// フォールバック: 白テクスチャ + material 色
				if (g_WhiteTexId != 0) {
					Texture_SetTexture(g_WhiteTexId);
				}
				else if (s_fallbackWhiteSRV) {
					ID3D11ShaderResourceView* srv = s_fallbackWhiteSRV.Get();
					g_pContext->PSSetShaderResources(0, 1, &srv);
				}
				else {
				}
				aiColor3D diffuse;
				aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
				Shader3d_SetMaterialDiffuse({ diffuse.r, diffuse.g, diffuse.b, 1.0f });
			}
		}
		else
		{
			Texture_SetTexture(g_WhiteTexId);
			aiColor3D diffuse;
			aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
			Shader3d_SetMaterialDiffuse({ diffuse.r, diffuse.g, diffuse.b, 1.0f });
		}

		// 描画
		unsigned int indexCount = model->AiScene->mMeshes[ModelNum]->mNumFaces * 3;
		if (indexCount == 0) {
			DBG("Mesh %u indexCount==0", ModelNum);
			continue;
		}
		g_pContext->DrawIndexed(indexCount, 0, 0);
	}
}



static void LogSceneBounds(const MODEL* model)
{
	DirectX::XMFLOAT3 minf{ FLT_MAX, FLT_MAX, FLT_MAX };
	DirectX::XMFLOAT3 maxf{ -FLT_MAX, -FLT_MAX, -FLT_MAX };

	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; ++m)
	{
		const aiMesh* mesh = model->AiScene->mMeshes[m];
		if (!mesh || !mesh->mVertices) continue;
		for (unsigned int v = 0; v < mesh->mNumVertices; ++v)
		{
			const aiVector3D& p = mesh->mVertices[v];
			minf.x = std::min(minf.x, p.x); minf.y = std::min(minf.y, p.y); minf.z = std::min(minf.z, p.z);
			maxf.x = std::max(maxf.x, p.x); maxf.y = std::max(maxf.y, p.y); maxf.z = std::max(maxf.z, p.z);
		}
	}

	DirectX::XMFLOAT3 center{ (minf.x + maxf.x) * 0.5f, (minf.y + maxf.y) * 0.5f, (minf.z + maxf.z) * 0.5f };
	DirectX::XMFLOAT3 extent{ (maxf.x - minf.x) * 0.5f, (maxf.y - minf.y) * 0.5f, (maxf.z - minf.z) * 0.5f };
}
