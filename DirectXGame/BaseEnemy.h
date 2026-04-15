#pragma once

#include "KamataEngine.h"
#include "AABB.h"

class Player;

class BaseEnemy {
public:
	virtual ~BaseEnemy() = default;

	virtual void OnCollision(Player* player) = 0;

	virtual void Update() = 0;

	virtual void Draw() = 0;

	virtual AABB GetAABB() = 0;

	virtual bool IsCollidionDisabled() const { return false; }

	virtual bool IsDead() const { return false; }
};
