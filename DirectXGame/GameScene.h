#pragma once
#include "BaseEnemy.h"
#include "CameraController.h"
#include "DeathParticles.h"
#include "Enemy.h"
#include "Fade.h"
#include "HitEffect.h"
#include "GuardEffect.h"
#include "BaseEffect.h"
#include "KamataEngine.h"
#include "MapChipField.h"
#include "Player.h"
#include "ShieldEnemy.h"
#include "Skydome.h"

// ゲームシーン
class StageManager;
class GameScene final {
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
	std::list<BaseEnemy*> enemies_;
	KamataEngine::Model* modelEnemy_ = nullptr;

	DeathParticles* deathParticles_ = nullptr;
	KamataEngine::Model* modelDeathParticles_ = nullptr;

	enum class Phase { kFadeIn, kPlay, kDead, kFadeOut };

	Phase phase_;

	bool finished_ = false;

	Fade* fade_ = nullptr;

	static const inline float kFadingTime = 0.75f;

	KamataEngine::Model* modelAttack_ = nullptr;
	std::list<BaseEffect*> effects_;

	KamataEngine::Model* modelShieldEnemy_ = nullptr;
	KamataEngine::Model* modelGuardEffect_ = nullptr;

	// リロード管理
	bool reloadRequested_ = false;

	StageManager* stageManager_ = nullptr;

public:
	~GameScene();
	// 初期化
	void Initialize(StageManager* stageManager);

	// 更新処理
	void Update();

	// 描画処理
	void Draw();

	void GenerateFieldObjects();

	void CheckAllCollisions();

	void ChangePhase();

	bool IsFinished() const { return finished_; }

	void UpdateEnemies();
	void UpdateCamera();
	void UpdateBlocks();
	void UpdateDeathParticles();
	void CreateHitEffect(KamataEngine::Vector3 spawnPoint);
	void CreateGuardEffect(KamataEngine::Vector3 spawnPoint);

	bool IsReloadRequested() const { return reloadRequested_; }
};
