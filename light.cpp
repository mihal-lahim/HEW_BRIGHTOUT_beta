#include "light.h"
#include "direct3d.h"

using namespace DirectX;

static ID3D11Buffer* g_pPSConstantBuffer0;
static ID3D11Buffer* g_pPSConstantBuffer1;

void Light_Initialize()
{
	auto pDevice = Direct3D_GetDevice();

	// ピクセルシェーダーのライティング用定数バッファの作成
	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // バインドフラグ

	buffer_desc.ByteWidth = sizeof(XMFLOAT4); // バッファのサイズ
	pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pPSConstantBuffer0);

	buffer_desc.ByteWidth = sizeof(DiffuseLight); // バッファのサイズ
	pDevice->CreateBuffer(&buffer_desc, nullptr, &g_pPSConstantBuffer1);
}

void Light_Finalize()
{
	SAFE_RELEASE(g_pPSConstantBuffer0);
	SAFE_RELEASE(g_pPSConstantBuffer1);
}

void Light_SetAmbient(const XMFLOAT4& color)
{
	// 定数バッファにambienにカラーをセット
	Direct3D_GetContext()->UpdateSubresource(g_pPSConstantBuffer0, 0, nullptr, &color, 0, 0);
	Direct3D_GetContext()->PSSetConstantBuffers(0, 1, &g_pPSConstantBuffer0);
}

void Light_SetDiffuse(const XMFLOAT3& color, const XMFLOAT3& direction)
{
	DiffuseLight light{};
	light.color = XMFLOAT4(color.x, color.y, color.z, 1.0f);
	light.direction = XMFLOAT4(direction.x, direction.y, direction.z, 0.0f);

	// 定数バッファにdiffuseセット
	Direct3D_GetContext()->UpdateSubresource(g_pPSConstantBuffer1, 0, nullptr, &light, 0, 0);
	Direct3D_GetContext()->PSSetConstantBuffers(1, 1, &g_pPSConstantBuffer1);
}

void Light_SetDiffuse(const DiffuseLight& light)
{
	// 定数バッファにdiffuseセット
	Direct3D_GetContext()->UpdateSubresource(g_pPSConstantBuffer1, 0, nullptr, &light, 0, 0);
	Direct3D_GetContext()->PSSetConstantBuffers(1, 1, &g_pPSConstantBuffer1);
}
