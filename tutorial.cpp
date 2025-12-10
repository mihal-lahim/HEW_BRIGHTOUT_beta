/*==============================================================================

   チュートリアル[tutorial.cpp]
														 Author : syota
														 Date   : 2025/12/10
--------------------------------------------------------------------------------

==============================================================================*/
#include "tutorial.h"
#include "texture.h"
#include "sprite.h"
#include "key_logger.h"
#include "scene.h"


static int g_TexTutorialId = -1;

void Tutorial_Initialize()
{
	g_TexTutorialId = Texture_Load(L"texture/tutorial.png");
}

void Tutorial_Finalize()
{
}

void Tutorial_Update(double elapsed_time)
{
	if (KeyLogger_IsTrigger(KK_ENTER))
	{
		Scene_SetNextScene(SCENE_GAME);
	}
}

void Tutorial_Draw()
{
	Sprite_Draw(g_TexTutorialId, 0.0f, 0.0f, 1.0f);
}
