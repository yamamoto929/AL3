#pragma once
#include "KamataEngine.h"
#include "Player.h"
#include "Vector3Operator.h"

class Player;
class CameraController {
public:
	enum class Mode {
		kFollow,
		kForcedScroll
	};
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(KamataEngine::Camera* camera);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	void SetTarget(Player* target) { target_ = target; }

	void Reset();

	// 矩形
	struct Rect {
		float left = 0.0f;
		// 左端
		float right = 1.0f;
		// 右端
		float bottom = 0.0f;
		// 下端
		float top = 1.0f;
		// 上端
	};

	void SetMovableArea(Rect area) { movableArea_ = area; };
	void SetMode(Mode mode) { mode_ = mode; }
	Mode GetMode() const{ return mode_; }

private:
	KamataEngine::Camera* camera_ = nullptr;
	Player* target_ = nullptr;

	KamataEngine::Vector3 targetOffset_ = KamataEngine::Vector3{0.0f, 0.0f, -15.0f};
	Rect movableArea_ = {0.0f, 100.0f, 0.0f, 100.0f};

	KamataEngine::Vector3 targetPosition_ = {};

	static inline const float kInterpolationRate = 0.3f;

	static inline const float kVelocityBias = 20.0f;

	static inline const Rect margin = {-2.0f, 2.0f, -2.0f, 2.0f};

	Mode mode_ = Mode::kFollow;
};
