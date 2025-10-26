#pragma once

#include <d3d9types.h>
#include <corecrt_math.h>

class Vector2
{
public:
	Vector2() : x(0.f), y(0.f)
	{

	}

	Vector2(double _x, double _y) : x(_x), y(_y)
	{

	}
	~Vector2()
	{

	}

	double x;
	double y;
};

class Vector3
{
public:
	Vector3() : x(0.f), y(0.f), z(0.f)
	{
	}

	Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z)
	{
	}
	~Vector3()
	{
	}

	double x;
	double y;
	double z;

	inline double Dot(Vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline double Distance(Vector3 v)
	{
		return double(sqrtf(powf(v.x - x, 2.0f) + powf(v.y - y, 2.0f) + powf(v.z - z, 2.0f)));
	}

	inline double Length() {
		return sqrt(x * x + y * y + z * z);
	}

	Vector3 operator+(Vector3 v)
	{
		return Vector3(x + v.x, y + v.y, z + v.z);
	}

	Vector3 operator-(Vector3 v)
	{
		return Vector3(x - v.x, y - v.y, z - v.z);
	}

	Vector3 operator*(double flNum) { return Vector3(x * flNum, y * flNum, z * flNum); }
};

#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif

inline static float rad2deg = (float)M_PI / 360.0f;
inline static float rad2deg2 = (float)M_PI / 180.0f;

namespace Engine
{

	struct Entity
	{
		uintptr_t PlayerState = 0;
		uintptr_t Pawn = 0;
		uintptr_t BoneArray = 0;
		uintptr_t Mesh = 0;
		uintptr_t CurrentWeapon = 0;
		
		Vector3 HeadBonePos3D{};
		Vector3 RootBonePos3D{};
		Vector2 HeadBonePos2D{};
		Vector2 RootBonePos2D{};
		
		char isDying = 0;
		char isDowned = 0;
		bool isVisible = false;

		int EntityID = -1;
		double WorldDist = -1.0f;
		double ScreenDist = -1.0f;
	};

	struct Camera
	{
		Vector3 Location;
		Vector3 Rotation;
		float FieldOfView;
	};

	struct FQuat
	{
		double x;
		double y;
		double z;
		double w;
	};

	struct FTransform
	{
		FQuat rot;
		Vector3 translation;
		Vector3 scale;

		D3DMATRIX ToMatrixWithScale() const
		{
			D3DMATRIX m{};
			m._41 = (float)translation.x;
			m._42 = (float)translation.y;
			m._43 = (float)translation.z;

			double x2 = rot.x + rot.x;
			double y2 = rot.y + rot.y;
			double z2 = rot.z + rot.z;

			double xx2 = rot.x * x2;
			double yy2 = rot.y * y2;
			double zz2 = rot.z * z2;

			m._11 = float((1.0 - (yy2 + zz2)) * scale.x);
			m._22 = float((1.0 - (xx2 + zz2)) * scale.y);
			m._33 = float((1.0 - (xx2 + yy2)) * scale.z);

			double yz2 = rot.y * z2;
			double wx2 = rot.w * x2;
			m._32 = float((yz2 - wx2) * scale.z);
			m._23 = float((yz2 + wx2) * scale.y);

			double xy2 = rot.x * y2;
			double wz2 = rot.w * z2;
			m._21 = float((xy2 - wz2) * scale.y);
			m._12 = float((xy2 + wz2) * scale.x);

			double xz2 = rot.x * z2;
			double wy2 = rot.w * y2;
			m._31 = float((xz2 + wy2) * scale.z);
			m._13 = float((xz2 - wy2) * scale.x);

			m._14 = 0.0f;
			m._24 = 0.0f;
			m._34 = 0.0f;
			m._44 = 1.0f;

			return m;
		}
	};

	
	inline D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
	{
		D3DMATRIX pOut;
		pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
		pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
		pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
		pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
		pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
		pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
		pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
		pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
		pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
		pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
		pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
		pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
		pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
		pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
		pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
		pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

		return pOut;
	}

	inline D3DMATRIX Matrix(Vector3 rot, Vector3 origin) {
		float radPitch = rot.x * rad2deg2;
		float radYaw = rot.y * rad2deg2;
		float radRoll = rot.z * rad2deg2;

		float SP = sinf(radPitch);
		float CP = cosf(radPitch);
		float SY = sinf(radYaw);
		float CY = cosf(radYaw);
		float SR = sinf(radRoll);
		float CR = cosf(radRoll);

		D3DMATRIX _matrix;
		_matrix.m[0][0] = CP * CY;
		_matrix.m[0][1] = CP * SY;
		_matrix.m[0][2] = SP;
		_matrix.m[0][3] = 0.f;

		_matrix.m[1][0] = SR * SP * CY - CR * SY;
		_matrix.m[1][1] = SR * SP * SY + CR * CY;
		_matrix.m[1][2] = -SR * CP;
		_matrix.m[1][3] = 0.f;

		_matrix.m[2][0] = -(CR * SP * CY + SR * SY);
		_matrix.m[2][1] = CY * SR - CR * SP * SY;
		_matrix.m[2][2] = CR * CP;
		_matrix.m[2][3] = 0.f;

		_matrix.m[3][0] = origin.x;
		_matrix.m[3][1] = origin.y;
		_matrix.m[3][2] = origin.z;
		_matrix.m[3][3] = 1.f;

		return _matrix;
	}
}