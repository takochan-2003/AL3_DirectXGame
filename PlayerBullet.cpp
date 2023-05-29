﻿#include"PlayerBullet.h"
#include <cassert>

void PlayerBullet::Initialize(Model* model, const Vector3& position,const Vector3& velocity){ 
	assert(model);
	model_ = model;
	textureHandle_ = TextureManager::Load("GENSEKI.png");
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	velocity_ = velocity ;
}

void PlayerBullet::Update() {

		worldTransform_.translation_=Add(worldTransform_.translation_, velocity_);

	worldTransform_.matWorld_ = MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);


	worldTransform_.TransferMatrix();
	//時間経過でデス
	if (--deathTimer_ <= 0)
	{
		isDead_ = true;
	}
	
}

void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}