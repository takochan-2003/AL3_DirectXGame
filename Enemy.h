#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include"Input.h"
#include"EnemyBullet.h"
#include <list>

class Player;


class Enemy {

	public:
	// デストラクタ
	~Enemy();

	//発射感覚
	static const int kFireInterval = 60;

	void Initialize(Model* model, uint32_t textureHndle);

	void Update();

	void Draw(ViewProjection& viewProjection);


	void Fire();

	enum class Phase {
		approach,
		Leave,
	};

	void Approach();

	Vector3 GetWorldPosition();

	//自キャラ
	Player* player_ = nullptr;

	void SetPlayer(Player* player) { player_ = player; }

	private:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
	Enemy* enemy_ = nullptr;
	Input* input_ = nullptr;
	Phase phase_ = Phase::approach;
	//発射タイマー
	int32_t fireTimer_ = 0; 
	// 弾
	std::list<EnemyBullet*> bullets_;
};