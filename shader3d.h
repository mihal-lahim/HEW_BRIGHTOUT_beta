/*==============================================================================

   3D用シェーダー [shader3d.h]
														 Author : Rintarou Sumi
														 Date   : 2025/10/16
--------------------------------------------------------------------------------

==============================================================================*/
#ifndef SHADER3D_H
#define	SHADER3D_H

#include <d3d11.h>
#include <DirectXMath.h>

// ピクセルシェーダー用マテリアル定数バッファ（カラーオーバーライド対応）
struct MaterialCB
{
    DirectX::XMFLOAT4 overrideColor;
    int useOverride = 0;    // 1でオーバーライドカラーを使用、0で通常
    int hasTex = 0;         // 1でテクスチャがバインドされている、0で無し
    int pad[2];             // 16バイトアライメント用パディング
};

bool Shader3d_Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
void Shader3d_Finalize();

void Shader3d_SetWorldMatrix(const DirectX::XMMATRIX& matrix);
void Shader3d_SetViewMatrix(const DirectX::XMMATRIX& matrix);
void Shader3d_SetProjectionMatrix(const DirectX::XMMATRIX& matrix);

void Shader3d_SetMaterialDiffuse(const DirectX::XMFLOAT4 color);
void Shader3d_SetMaterialCB(const MaterialCB& mtlCB);

void Shader3d_Begin();

#endif // SHADER3D_H
