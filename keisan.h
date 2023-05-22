﻿#include "Matrix4x4.h"
#include "Vector3.h"
#define _USE_MATH_DEFINES
#include <math.h>

Vector3 Add(Vector3 vector1, Vector3 vector2);
Matrix4x4 MakeRotateXMatrix(float radian);
Matrix4x4 MakeRotateYMatrix(float radian);
Matrix4x4 MakeRotateZMatrix(float radian);
Matrix4x4 Multiply(Matrix4x4 m1, Matrix4x4 m2);
Matrix4x4 MakeAffineMatrix(const Vector3& scale, Vector3& rotate, const Vector3& translate);