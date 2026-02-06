#ifndef MESH_H
#define MESH_H

#include "Resource.h"
#include <DirectXMath.h>



class Mesh : public Resource
{
public:
	// í∏ì_ç\ë¢ëÃ
	struct MeshVertex
	{
		DirectX::XMFLOAT3 position = {};
		DirectX::XMFLOAT4 color = {};
		DirectX::XMFLOAT3 normal = {};
		DirectX::XMFLOAT2 uv = {};
	};
};


#endif
