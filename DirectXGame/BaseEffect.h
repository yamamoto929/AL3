#pragma once
#include "KamataEngine.h"
#include <array>

class BaseEffect {
protected:

	float animCount_ = 0.0f;
	
	KamataEngine::Model* model_ = nullptr;
	KamataEngine::Camera* camera_ = nullptr;

public:
	virtual ~BaseEffect() = default;

	// 初期化時にモデルとカメラを受け取る
	virtual void Initialize(KamataEngine::Vector3 spawnPoint, KamataEngine::Model* model, KamataEngine::Camera* camera)=0;
	virtual void Update()=0;
	virtual void Draw()=0;

	virtual bool IsDead()const = 0;
};