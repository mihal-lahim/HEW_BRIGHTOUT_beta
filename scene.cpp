#include "scene.h"
#include "title.h"
#include "game.h"
#include "result.h"

static Scene g_Scene = SCENE_TITLE;// デバッグの時はこのシーンをゲームにしておく
static Scene g_SceneNext = g_Scene;

// 関数ポインタの型定義
typedef void (*FuncVoid)(void);
typedef void (*FuncDouble)(double);

static FuncVoid g_Initialize[]{
	Title_Initialize,
	//Tutorial_Initialize, 
	Game_Initialize,
	//FirstResult_Initialize,
	//SecondResult_Initialize,
	Result_Initialize,
	//NameInput_Initialize
};

static FuncVoid g_Finalize[]{
	Title_Finalize,
	//Tutorial_Finalize, 
	Game_Finalize,
	//FirstResult_Finalize,
	//SecondResult_Finalize,
	Result_Finalize,
	//NameInput_Finalize
};

static FuncDouble g_Update[]{
	Title_Update,
	//Tutorial_Update, 
	Game_Update,
	//FirstResult_Update,
	//SecondResult_Update,
	Result_Update,
	//NameInput_Update
};

static FuncVoid g_Draw[]{
	Title_Draw,
	//Tutorial_Draw, 
	Game_Draw,
	//FirstResult_Draw,
	//SecondResult_Draw,
	Result_Draw,
	//NameInput_Draw
};


void Scene_Initialize()
{
	g_Initialize[g_Scene]();
}

void Scene_Finalize()
{
	g_Finalize[g_Scene]();
}

void Scene_Update(double elapsed_time)
{
	g_Update[g_Scene](elapsed_time);
}

void Scene_Draw()
{
	g_Draw[g_Scene]();
}

void Scene_SetNextScene(Scene scene)
{
	g_SceneNext = scene;
}

void Scene_ChangeScene()
{
	if (g_Scene != g_SceneNext)
	{
		Scene_Finalize();// 現在のシーンの後片付け
		g_Scene = g_SceneNext;// 今のシーンを次のシーンとする

		Scene_Initialize();// 次のシーンの初期化
	}
}
