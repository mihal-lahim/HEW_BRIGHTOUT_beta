#include "SpriteRenderer.h"
#include "GameObject.h"
#include "Texture.h"
#include "Transform.h"

void SpriteRenderer::EnsureVertexBuffer(GraphicsDevice& device)
{
	if (m_vertexBuffer)
	{
		return;
	}

	Vertex vertices[] = {
		{ { -0.5f, -0.5f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
		{ {  0.5f, -0.5f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } },
		{ { -0.5f,  0.5f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
		{ {  0.5f,  0.5f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
	};

	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initData{};
	initData.pSysMem = vertices;

	device.GetDevice()->CreateBuffer(&bufferDesc, &initData, m_vertexBuffer.GetAddressOf());
}

void SpriteRenderer::Render(GraphicsDevice& device)
{
	if (!m_vertexShader || !m_materialInstance)
	{
		return;
	}

	EnsureVertexBuffer(device);

	Transform& tf = gameObject().transform();
	DirectX::XMMATRIX worldMatrix =
		DirectX::XMMatrixScaling(tf.scale().x, tf.scale().y, 1.0f) *
		tf.rotation().ToXMMATRIX() *
		DirectX::XMMatrixTranslation(tf.position().x, tf.position().y, tf.position().z);

	PerObjectCB::CBData objectData{};
	DirectX::XMStoreFloat4x4(&objectData.WorldMatrix, DirectX::XMMatrixTranspose(worldMatrix));
	m_vertexShader->UpdatePerObjectCB(device, objectData);
	m_vertexShader->Bind(device);

	m_materialInstance->Apply(device);

	if (m_texture)
	{
		m_texture->BindResource(device);
	}

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	auto* context = device.GetDeviceContext();
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->Draw(4, 0);
}
