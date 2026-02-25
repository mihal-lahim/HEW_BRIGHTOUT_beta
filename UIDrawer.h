#pragma once

#include <string>
#include "GameObject.h"
#include "Renderer.h"

namespace UI
{
	GameObject* CreateUI(GameObject* owner, const std::wstring& texturePath, const Vector3& position, const Vector3& scale, const std::string& vsPath, const std::string& psPath);
}