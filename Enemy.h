#pragma once
#include "EnemyBullet.h"
#include "Input.h"
#include "Model.h"
#include "WorldTransform.h"

#include <list>

class Player;

class GameScene;

class Enemy {

public:	// メンバ関数
	// デストラクタ
	~Enemy();

	void Initialize(Model* model, uint32_t textureHndle);

	void Update();

	void Draw(ViewProjection& viewProjection);

	Vector3 GetWorldPosition();
	void SetPlayer(Player* player) { player_ = player; }

	//コールバック関数
	void OnCollision();

	void SetGameScene(GameScene* gamescene) { gamescene_ = gamescene; }

private:	// メンバ関数
	void Fire();
	void shot(); // メンバ関数
	void run();
	void Approach();
	static void (Enemy::*enemyMove[])(); // メンバ関数ポインタ


private:	// メンバ変数
	// 発射感覚
	static const int kFireInterval = 60;

	// 自キャラ
	enum class Phase {
		approach,
		Leave,
	};

	Player* player_ = nullptr;

	WorldTransform worldTransform_;
	Model* model_ = nullptr;
	uint32_t textureHandle_ = 0u;
	Enemy* enemy_ = nullptr;
	Input* input_ = nullptr;
	Phase phase_ = Phase::approach;
	// 発射タイマー
	int32_t fireTimer_ = 0;
	
	//ゲームシーン
	GameScene* gamescene_ = nullptr;

	

};