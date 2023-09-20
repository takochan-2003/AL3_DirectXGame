﻿#include "Enemy.h"
#include <cassert>
#define _USE_MATH_DEFINES
#include "ImGuiManager.h"
#include "keisan.h"
#include <math.h>
#include "Player.h"
#include "GameScene.h"

void Enemy::Initialize(Model* model, const Vector3& position, const Vector3& velocity){

	assert(model);

	model_ = model;

	textureHandle_ = TextureManager::Load("KOKUDO.png");


	enemy_ = new Enemy();

	worldTransform_.Initialize();
	worldTransform_.scale_ = {20.0f, 20.0f, 20.0f};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.translation_ = position;

	Approach();
	input_ = Input::GetInstance();

	//enemyMove = &Enemy::shot; // ポインタに関数のアドレスを代入

}



void Enemy::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}

//フェーズの関数テーブル
void (Enemy::*Enemy::enemyMove[])() = {
    &Enemy::shot, // 接近
    &Enemy::run   // 離脱
};


void Enemy::Update(){

	Vector3 move = {0, 0, -0.2f};
	Vector3 leave = {0.6f, 0.6f, -1.0f};
	const float kCharacterSpeed = 0.2f;

	////移動処理(SWitch)
	//switch (phase_) { case Phase::approach:
	//default:
	//	//移動(ベクトルを加算)
	//	worldTransform_.translation_ = Add(worldTransform_.translation_, move);
	//	if (worldTransform_.translation_.z < 100.0f) {
	//		phase_ = Phase::Leave;
	//	}
	//	break;
	//case Phase::Leave:
	//	//P7
	//	worldTransform_.translation_ = Add(worldTransform_.translation_, leave);
	//	break;
	//}

	//2-7の実践
	(this->*enemyMove[static_cast<size_t>(phase_)])(); // shotを呼び出す

	float EnemyPos[3] = {
	    worldTransform_.translation_.x, worldTransform_.translation_.y,
	    worldTransform_.translation_.z};

	// 画面の座標を表示
	ImGui::Begin("Enemy");
	ImGui::SliderFloat3("EnemyPos", EnemyPos, 600.0f, -600.0f);
	ImGui::End();

	worldTransform_.translation_.x = EnemyPos[0];
	worldTransform_.translation_.y = EnemyPos[1];
	worldTransform_.translation_.z = EnemyPos[2];

	worldTransform_.translation_ = Add(worldTransform_.translation_, move);


	// スケーリング行列を宣言
	Matrix4x4 matScale;
	matScale.m[0][0] = worldTransform_.scale_.x;
	matScale.m[1][1] = worldTransform_.scale_.y;
	matScale.m[2][2] = worldTransform_.scale_.z;
	matScale.m[3][3] = 1;

	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(worldTransform_.rotation_.x);
	Matrix4x4 rotateYMatriy = MakeRotateYMatrix(worldTransform_.rotation_.y);
	Matrix4x4 rotateZMatriz = MakeRotateZMatrix(worldTransform_.rotation_.z);
	Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatriy, rotateZMatriz));

	// 平行移動行列を宣言
	Matrix4x4 matTrans;
	matTrans.m[0][0] = 1;
	matTrans.m[1][1] = 1;
	matTrans.m[2][2] = 1;
	matTrans.m[3][3] = 1;
	matTrans.m[3][0] = worldTransform_.translation_.x;
	matTrans.m[3][1] = worldTransform_.translation_.y;
	matTrans.m[3][2] = worldTransform_.translation_.z;

	worldTransform_.matWorld_ = MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();

	//接近フェーズの更新関数
	--fireTimer_;
	if (fireTimer_ < 0) {
		Fire();
		fireTimer_ = kFireInterval;
	}

	for (EnemyBullet* bullet : bullets_) {
		bullet->Update();
	}
}

Enemy::~Enemy() {}

void Enemy::Approach() {
	//発射タイマーを初期化
	fireTimer_ = 30;

}

void Enemy::Fire() {
	assert(player_);
		const float kBulletSpeed = -0.3f;
	    Vector3 speed = {2.0f, 2.0f, 2.0f};
	    Vector3 velocity(0, 0, kBulletSpeed);

	    Vector3 subVector = Subtract(player_->GetWorldPosition(),GetWorldPosition());
	    subVector=Normalize(subVector);
	    subVector=VectorMultiply(subVector, speed);
	    
		velocity = subVector;

		// 弾を生成し、初期化
		EnemyBullet* newBullet = new EnemyBullet();
		newBullet->Initialize(model_, worldTransform_.translation_, velocity);
		// 弾を登録する
	    gamescene_->AddEnemyBullet(newBullet);
	
}

void Enemy::shot() {
	    Vector3 move = {0, 0, -0.2f};
	    worldTransform_.translation_ = Add(worldTransform_.translation_, move);
	    if (worldTransform_.translation_.z < 100.0f) {
		phase_ = Phase::Leave;
	    }
}

void Enemy::run() { 
	Vector3 leave = {0.6f, 0.6f, -1.0f};
	worldTransform_.translation_ = Add(worldTransform_.translation_, leave);
}

Vector3 Enemy::GetWorldPosition() {
	    Vector3 worldPos;

	    worldPos.x = worldTransform_.matWorld_.m[3][0];
	    worldPos.y = worldTransform_.matWorld_.m[3][1];
	    worldPos.z = worldTransform_.matWorld_.m[3][2];

	    return worldPos;
}

void Enemy::OnCollision() {}

