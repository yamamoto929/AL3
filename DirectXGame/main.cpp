#include "GameScene.h"
#include "KamataEngine.h"
#include "TitleScene.h"
#include "StageManager.h"
#include <Windows.h>

using namespace KamataEngine;

TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;

enum class Scene { kUnknown = 0, kTitle, kGame };

Scene scene = Scene::kUnknown;

void ChangeScene();
void UpdateScene();
void DrawScene();
StageManager* stageManager = nullptr;


// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize();

	stageManager = new StageManager;
	stageManager->LoadStageFiles();

	scene = Scene::kTitle;
	titleScene = new TitleScene;

	titleScene->Initialize();

	// ゲームシーンのインスタンス作成
	gameScene = new GameScene;

	// ゲームシーンの初期化
	gameScene->Initialize(stageManager);

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	ImGuiManager* imguiManager = ImGuiManager::GetInstance();

	// メインループ
	while (true) {
		// エンジンの更新
		if (KamataEngine::Update()) {
			break;
		}
		imguiManager->Begin();

		ChangeScene();
		UpdateScene();

		imguiManager->End();
		// 描画開始
		dxCommon->PreDraw();

		DrawScene();

		imguiManager->Draw();
		// 描画終了
		dxCommon->PostDraw();
	}

	// エンジンの終了処理
	KamataEngine::Finalize();

	delete titleScene;
	delete gameScene;
	delete stageManager;
	// nullptrの代入
	gameScene = nullptr;
	titleScene = nullptr;
	stageManager = nullptr;

	return 0;
}

void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			scene = Scene::kGame;
			delete titleScene;
			titleScene = nullptr;

			gameScene = new GameScene;
			gameScene->Initialize(stageManager);
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			scene = Scene::kTitle;
			delete gameScene;
			gameScene = nullptr;

			titleScene = new TitleScene;
			titleScene->Initialize();
		} else if (gameScene->IsReloadRequested()) {
			delete gameScene;
			gameScene = nullptr;
			gameScene = new GameScene;
			gameScene->Initialize(stageManager);
		}
		break;
	}
};

void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	}
};

void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	}
};
