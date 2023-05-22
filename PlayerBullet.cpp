#include"PlayerBullet.h"
#include <cassert>

void PlayerBullet::Initialize(Model* model, const Vector3& position){ 
	assert(model);
	model_ = model;
	textureHandle_ = TextureManager::Load("GENSEKI.png");
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

}

void PlayerBullet::Update() {
	worldTransform_.matWorld_ = MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();
}

void PlayerBullet::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
}