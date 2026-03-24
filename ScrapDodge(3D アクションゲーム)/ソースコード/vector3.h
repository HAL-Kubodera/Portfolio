/*============================================================
	[vector3.h]
	▼題名 : オリジナル ベクター構造体
	▼DATE : 2026/03/18
	▼AUTHOR : 久保寺 俊介
============================================================*/
#ifndef _VECTOR3_H_
#define _VECTOR3_H_

#include<math.h>

class Vector3
{
public:
	float x, y, z;

	/*constexpr Vector3(float x_, float y_, float z_)
		: x(x_), y(y_), z(z_) {
	}*/

	//コンストラクタ
	constexpr Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
	constexpr Vector3(const Vector3& a) : x(a.x), y(a.y), z(a.z) {}
	constexpr Vector3(float nx, float ny, float nz) : x(nx), y(ny), z(nz) {}

	//代入
	Vector3 &operator =(const Vector3& a)
	{
		x = a.x;
		y = a.y;
		z = a.z;

		return *this;
	}

	//等しさのチェック
	bool operator ==(const Vector3& a) const
	{
		return x == a.x && y == a.y && z == a.z;
	}

	bool operator !=(const Vector3& a) const
	{
		return x != a.x || y != a.y || z != a.z;
	}

	//ベクトル操作

	//ベクトルを0にする
	void zero() { x = y = z = 0; }

	//単項式のマイナスは反転したベクトルを返す
	Vector3 operator -() const { return Vector3(-x, -y, -z); }

	//二項式の+と-はベクトルを加算し、減算する
	Vector3 operator +(const Vector3& a) const
	{
		return Vector3(x + a.x, y + a.y, z + a.z);
	}

	Vector3 operator -(const Vector3& a) const
	{
		return Vector3(x - a.x, y - a.y, z - a.z);
	}

	//スカラーによる乗算と除算
	Vector3 operator * (float a) const
	{
		return Vector3(x * a, y * a, z * a);
	}

	Vector3 operator / (float a) const
	{
		float oneOverA = 1.0f / a;

		return Vector3(x * oneOverA, y * oneOverA, z * oneOverA);
	}

	//組み合わせ代入演算
	Vector3& operator +=(const Vector3& a)
	{
		x += a.x;
		y += a.y;
		z += a.z;

		return *this;
	}

	Vector3& operator -=(const Vector3& a)
	{
		x -= a.x;
		y -= a.y;
		z -= a.z;

		return *this;
	}

	Vector3& operator *=(float a)
	{
		x *= a;
		y *= a;
		z *= a;

		return *this;
	}

	Vector3& operator /=(float a)
	{
		float oneOverA = 1.0f / a;

		x += oneOverA;
		y += oneOverA;
		z += oneOverA;

		return *this;
	}

	//正規化
	void normalize()
	{
		float magSq = x * x + y * y + z * z;

		if (magSq > 0.0f) //ゼロ除算チェック
		{
			float oneOverMag = 1.0f / sqrtf(magSq);
			x *= oneOverMag;
			y *= oneOverMag;
			z *= oneOverMag;
		}
	}

	//内積
	float operator *(const Vector3& a) const
	{
		return x * a.x + y * a.y + z * a.z;
	}

	float operator[](int index) const {
		if (index == 0) return x;
		if (index == 1) return y;
		return z;
	}

	float length() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	static float dot(const Vector3& a, const Vector3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	static Vector3 cross(const Vector3& a, const Vector3& b)
	{
		Vector3 ret;
		ret.x = a.y * b.z - a.z * b.y;
		ret.y = a.z * b.x - a.x * b.z;
		ret.z = a.x * b.y - a.y * b.x;

		return ret;
	}
};

#endif // _VECTOR3_H_