﻿#include "Enemy.h"
#include <cassert>
#define _USE_MATH_DEFINES
#include "ImGuiManager.h"
#include "keisan.h"
#include <math.h>
#include "Player.h"

void Enemy::Initialize(Model* model, uint32_t textureHndle) {
	assert(model);
	model_ = model;
	textureHandle_ = TextureManager::Load("KOKUDO.png");
	worldTransform_.Initialize();
	input_ = Input::GetInstance();
	worldTransform_.scale_ = {20.0f, 20.0f, 20.0f};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.translation_ = {40.0f, 0.0f, 300.0f};
	Approach();
	// 解放
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}

}

void Enemy::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);

	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}

void Enemy::Update(){

	bullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	Vector3 move = {0, 0, -0.2f};
	Vector3 leave = {-0.3f, -0.3f, 0.5f};
	const float kCharacterSpeed = 0.2f;

	//移動処理(SWitch)
	switch (phase_) { case Phase::approach:
	default:
		//移動(ベクトルを加算)
		worldTransform_.translation_ = Add(worldTransform_.translation_, move);
		if (worldTransform_.translation_.z < 100.0f) {
			phase_ = Phase::Leave;
		}
		break;
	case Phase::Leave:
		//P7
		worldTransform_.translation_ = Add(worldTransform_.translation_, leave);
		break;
	}

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

void Enemy::Approach() {
	//発射タイマーを初期化
	fireTimer_ = 30;

}

void Enemy::Fire() {
	assert(player_);
		const float kBulletSpeed = -0.3f;
	    Vector3 speed = {2.0f, 2.0f, 2.0f};
	    Vector3 velocity(0, 0, kBulletSpeed);

	    player_->GetWorldPosition();
		GetWorldPosition();
	    Vector3 subVector = Subtract(player_->GetWorldPosition(),GetWorldPosition());
	    subVector=Normalize(subVector);
	    subVector=VectorMultiply(subVector, speed);
	    
		velocity = subVector;

		
		// 向き
		//velocity = TransformNormal(velocity, worldTransform_.matWorld_);
		// 弾を生成し、初期化
		EnemyBullet* newBullet = new EnemyBullet();
		newBullet->Initialize(model_, worldTransform_.translation_, velocity);
		// 弾を登録する
		bullets_.push_back(newBullet);
	
}

Vector3 Enemy::GetWorldPosition() {
	    Vector3 worldPos;

	    worldPos.x = worldTransform_.matWorld_.m[3][0];
	    worldPos.y = worldTransform_.matWorld_.m[3][1];
	    worldPos.z = worldTransform_.matWorld_.m[3][2];

	    return worldPos;
}

Enemy::~Enemy() {}