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
#include "controller.h"


static Controller* g_Controller = nullptr;

static int g_TexTutorialId = -1;

void Tutorial_Initialize()
{
	g_TexTutorialId = Texture_Load(L"texture/tutorial.png");
	g_Controller = new Controller(0);
}

void Tutorial_Finalize()
{
	delete g_Controller;
	g_Controller = nullptr;
}

void Tutorial_Update(double/* elapsed_time*/)
{
	g_Controller->Update();
	if (KeyLogger_IsTrigger(KK_ENTER)|| g_Controller->WasPressed(Controller::BUTTON_A))
	{
		Scene_SetNextScene(SCENE_NAME_INPUT);
	}
}

void Tutorial_Draw()
{
	Sprite_Draw(g_TexTutorialId, 0.0f, 0.0f, 1.0f);
}
