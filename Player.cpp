#include "Player.h"
#include <cassert>
#define _USE_MATH_DEFINES
#include "ImGuiManager.h"
#include "keisan.h"
#include <math.h>



void Player::Initialize(Model* model, uint32_t textureHndle,Vector3 playerPotision) {
	assert(model);
	model_ = model;
	
	worldTransform_.Initialize();
	input_ = Input::GetInstance();
	worldTransform_.scale_ = {3.0f, 3.0f, 3.0f};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.translation_ = {0.0f, 0.0f, 40.0f};
	//解放
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}

	//3Dレティクルのワールドトランスフォーム初期化
	worldTransform3DReticle_.Initialize();

}

void Player::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
	model_->Draw(worldTransform3DReticle_, viewProjection);

	//if (bullet_) {
	//	bullet_->Draw(viewProjection);
	//}
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw(viewProjection);
	}
}

void Player::Update() {

	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
	});

	Vector3 move = {0, 0, 0};
	const float kCharacterSpeed = 0.2f;

	if (input_->PushKey(DIK_LEFT)) {

		move.x -= kCharacterSpeed;
	} else if (input_->PushKey(DIK_RIGHT)) {

		move.x += kCharacterSpeed;
	}
	if (input_->PushKey(DIK_UP)) {

		move.y += kCharacterSpeed;
	} else if (input_->PushKey(DIK_DOWN)) {

		move.y -= kCharacterSpeed;
	}

	const float kRotSpeed = 0.02f;

	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y += kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	}

	float playerPos[3] = {
	    worldTransform_.translation_.x, worldTransform_.translation_.y,
	    worldTransform_.translation_.z};

	// 画面の座標を表示
	ImGui::Begin("Player");
	ImGui::SliderFloat3("playerPos", playerPos, -28.0f, 28.0f);
	ImGui::End();

	worldTransform_.translation_.x = playerPos[0];
	worldTransform_.translation_.y = playerPos[1];
	worldTransform_.translation_.z = playerPos[2];

	worldTransform_.translation_ = Add(worldTransform_.translation_, move);
	

	// 移動限界座標
	const float kMoveLimitX = 28;
	const float kMoveLimitY = 14;

	// 範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);
	
	worldTransform_.UpdateMatrix();

	//自機のワールド座標から3Dレティクルのワールド座標を計算
	{
		//自機から3Dレティクルへの距離
		const float kDistancePlayerTo3DReticle = 50.0f;
		//自機から3Dレティクルへのオフセット（Z+向き）
		Vector3 offset = {0, 0, 1.0f};
		//自機のワールド行列の回転を反映
		offset = VectorMatrixMultiply(offset, worldTransform_.matWorld_);
		//ベクトルの長さを整える
		offset = VectorIndexMultiply(Normalize(offset),kDistancePlayerTo3DReticle);
		//3Dレティクルの座標を設定
		worldTransform3DReticle_.translation_ = offset;
		//ワールド行列の更新と転送
		worldTransform_.matWorld_ = MakeAffineMatrix(
		    worldTransform_.scale_,
			worldTransform_.rotation_,
			worldTransform_.translation_);
		
		worldTransform_.TransferMatrix();
	}
	
	// 攻撃処理
	Attack();
	for (PlayerBullet* bullet : bullets_) {
		bullet->Update();
	}
}

void Player::Rotate() {
	const float kRotSpeed = 0.02f;
	if (input_->PushKey(DIK_A)) {
		worldTransform_.rotation_.y -= kRotSpeed;
	} else if (input_->PushKey(DIK_D)) {
		worldTransform_.rotation_.x += kRotSpeed;
	}
}

void Player::Attack() {
	if (input_->TriggerKey(DIK_SPACE)) {
		
		const float kBulletSpeed = 1.0f;
		Vector3 velocity(0, 0, 0);
		//向き
		//velocity = TransformNormal(velocity, worldTransform_.matWorld_);
		velocity = VectorSubtract(worldTransform3DReticle_.translation_,worldTransform_.translation_);
		velocity = VectorIndexMultiply(Normalize(velocity),kBulletSpeed);
		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, GetWorldPosition(), velocity);
		// 弾を登録する
		//bullet_ = newBullet;
		bullets_.push_back(newBullet);
	}
}

Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
}

void Player::OnCollision() {}

//親子関係を結ぶ
void Player::Setparent(const WorldTransform* parent) { worldTransform_.parent_ = parent; }

Player::~Player() {}

