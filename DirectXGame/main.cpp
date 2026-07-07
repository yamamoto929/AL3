#include "GameScene.h"
#include "KamataEngine.h"
#include "StageManager.h"
#include "TitleScene.h"
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <string>

using namespace KamataEngine;

TitleScene* titleScene = nullptr;
GameScene* gameScene = nullptr;

enum class Scene { kUnknown = 0, kTitle, kGame };

Scene scene = Scene::kUnknown;

void ChangeScene();
void UpdateScene();
void DrawScene();
void LoadDebugSettings();
StageManager* stageManager = nullptr;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンの初期化
	KamataEngine::Initialize();

	stageManager = new StageManager;
	stageManager->LoadStageFiles();
	#ifdef _DEBUG
	LoadDebugSettings();
	scene = Scene::kGame;
	// ゲームシーンのインスタンス作成
	gameScene = new GameScene;

	// ゲームシーンの初期化
	gameScene->Initialize(stageManager);
	#else
	scene = Scene::kTitle;
	titleScene = new TitleScene;
	titleScene->Initialize();
	#endif
	
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

void LoadDebugSettings() {
	const std::string filepath = "DebugSettings.ini";

	std::ifstream file;
	file.open(filepath);

	assert(file.is_open());

	std::string line;

	while (std::getline(file, line)) {
		std::stringstream lineStream(line);

		std::string key;
		std::string value;

		lineStream >> key >> value;

		if (key == "InitialStage") {
			stageManager->SetCurrentStageIndexByName(value);
		}
	}

	file.close();
}