#pragma once
#include "CameraController.h"
#include "DeathParticles.h"
#include "Enemy.h"
#include "Fade.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "Skydome.h"
#include "HitEffect.h"


// ゲームシーン
class GameScene {
private:
	uint32_t textureHandle_ = 0;
	uint32_t textureHandleBlock_ = 0;

	KamataEngine::Model* modelPlayer_ = nullptr;
	KamataEngine::Model* modelBlock_ = nullptr;
	KamataEngine::Model* modelHitEffect_ = nullptr;

	KamataEngine::WorldTransform* worldTransform_;
	KamataEngine::Camera* camera_;

	Player* player_ = nullptr;

	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	bool isDebugCameraActive_ = false;
	KamataEngine::DebugCamera* debugCamera_ = nullptr;

	Skydome* skydome_ = nullptr;

	KamataEngine::Model* modelSkydome_ = nullptr;

	MapChipField* mapChipField_ = nullptr;

	CameraController* cameraController_ = nullptr;

	const uint32_t kEnemyMax = 1;
	std::list<Enemy*> enemies_;
	KamataEngine::Model* modelEnemy_ = nullptr;

	DeathParticles* deathParticles_ = nullptr;
	KamataEngine::Model* modelDeathParticles_ = nullptr;

	enum class Phase { kFadeIn, kPlay, kDead, kFadeOut };

	Phase phase_;

	bool finished_ = false;

	Fade* fade_ = nullptr;

	static const inline float kFadingTime = 0.75f;

	KamataEngine::Model* modelAttack_;

public:
	~GameScene();
	// 初期化
	void Initialize();

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	void GenerateBlocks();

	void CheckAllCollisions();

	void ChangePhase();

	bool IsFinished() const { return finished_; }

	void UpdateEnemies();
	void UpdateCamera();
	void UpdateBlocks();
	void UpdateDeathParticles();
};
