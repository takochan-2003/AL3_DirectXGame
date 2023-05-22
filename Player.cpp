#include "player.h"
#include <cassert>
#define _USE_MATH_DEFINES
#include "ImGuiManager.h"
#include "keisan.h"
#include <math.h>

void Player::Initialize(Model* model, uint32_t textureHndle) {
	assert(model);
	model_ = model;
	textureHandle_ = textureHndle;
	worldTransform_.Initialize();
	input_ = Input::GetInstance();
	worldTransform_.scale_ = {5.0f, 5.0f, 5.0f};
	worldTransform_.rotation_ = {0.0f, 0.0f, 0.0f};
	worldTransform_.translation_ = {0.0f, 0.0f, 0.0f};
}

void Player::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, textureHandle_);
	if (bullet_) {
		bullet_->Draw(viewProjection);
	}
}

void Player::Update() {

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
	// ImGui::InputFloat3("playerPos", playerPos);
	ImGui::SliderFloat3("playerPos", playerPos, -28.0f, 28.0f);
	ImGui::End();

	worldTransform_.translation_.x = playerPos[0];
	worldTransform_.translation_.y = playerPos[1];
	worldTransform_.translation_.z = playerPos[2];

	worldTransform_.translation_ = Add(worldTransform_.translation_, move);
	// worldTransform_.rotation_ = Add(worldTransform_.translation_, move);

	// 移動限界座標
	const float kMoveLimitX = 28;
	const float kMoveLimitY = 14;

	// 範囲を超えない処理
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.y = max(worldTransform_.translation_.y, -kMoveLimitY);
	worldTransform_.translation_.y = min(worldTransform_.translation_.y, +kMoveLimitY);

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

	// worldTransform_.matWorld_ = Multiply(matScale, Multiply(rotateXYZMatrix,matTrans));
	worldTransform_.matWorld_ = MakeAffineMatrix(
	    worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	worldTransform_.TransferMatrix();

	// 攻撃処理
	Attack();
	if (bullet_) {
		bullet_->Update();
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
		// 弾を生成し、初期化
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(model_, worldTransform_.translation_);
		// 弾を登録する
		bullet_ = newBullet;
	}
}

// worldTransform_.translation_ += move;
//
//
//
//
//
// }