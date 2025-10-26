#pragma once

#include "offsets.hpp"
#include "../features/util/util.hpp"
#include "../../memory/memory.hpp"
#include "../config.h"

#include <memory>
#include <cstdint>
#include <d3dx9math.h>

namespace PtrCache
{
	inline uintptr_t Gworld = 0;
	inline uintptr_t GameInstance = 0;
	inline uintptr_t Player = 0;
	inline uintptr_t Mesh = 0;
	inline uintptr_t PlayerState = 0;
	inline uintptr_t RootComponent = 0;
	inline uintptr_t LocalPlayers = 0;
	inline uintptr_t PlayerController = 0;
	inline uintptr_t CameraLocation = 0;
	inline uintptr_t CameraRotation = 0;
	inline uintptr_t viewMatrix = 0;
	inline uintptr_t viewState = 0;

	inline Engine::Camera* vCamera = nullptr;
	inline Engine::Entity Target{};
}

// Hashmap for local bone pos tied to playerid
struct BoneCache
{
private:
	std::unordered_map<int32_t, Engine::FTransform> m_BoneMap;

public:
	inline void Set(int32_t boneID, const Engine::FTransform& position)
	{
		m_BoneMap[boneID] = position;
	}

	inline bool Get(int32_t boneID, Engine::FTransform& outPosition) const
	{
		if (m_BoneMap.empty())
			return false;
		
		auto it = m_BoneMap.find(boneID);
		if (it != m_BoneMap.end())
		{
			outPosition = it->second;
			return true;
		}

		return false;
	}

	inline void Clear()
	{
		if (!m_BoneMap.empty())
			m_BoneMap.clear();
	}

	inline void Reserve(size_t count)
	{
		m_BoneMap.reserve(count);
	}
};

// precalculate
inline std::shared_ptr<driver::c_Memory> _Memory = nullptr;

namespace sdk
{
	inline static BoneCache boneCache{};

	inline auto isVisible(uintptr_t Mesh) -> bool
	{
		if (!Mesh)
			return false;

		double Seconds = _Memory->Read<double>(PtrCache::Gworld + 0x198); // UWorld::LastRenderTime
		float LastRenderTime = _Memory->Read<float>(Mesh + 0x32C); // UPrimitiveComponent::LastRenderTimeOnScreen

		if (!Seconds || !LastRenderTime)
			return false;

		return (Seconds - LastRenderTime <= 0.06f);
	}

	inline auto GetBoneWithRotation(uintptr_t mesh, int id, int32_t playerId) -> Vector3
	{
		uintptr_t boneArray = _Memory->Read<uintptr_t>(mesh + 0x5E8); // bone array offset off uc
		
		Engine::FTransform BoneTransform{};

		BoneTransform = _Memory->ReadBuffer<Engine::FTransform>(boneArray + id * 0x50); // size of FTransform is 0x50 (80bytes)
		
		Engine::FTransform ComponentToWorld = _Memory->ReadBuffer<Engine::FTransform>(mesh + 0x1e0); // got 1e0 from memdumping
		
		// if cant get bone array try to get last bone local pos
		if (id > 0 && boneArray)
		{
			boneCache.Set(playerId, BoneTransform);
		}
		else if (id > 0 && !boneArray)
		{
			boneCache.Get(playerId, BoneTransform);
		}

		D3DMATRIX Matrix = Engine::MatrixMultiplication(BoneTransform.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());

		return Vector3(Matrix._41, Matrix._42, Matrix._43);
	}
	
	// Eular Angles
	
	inline auto GetViewAngles() -> Engine::Camera
	{
		Engine::Camera cam;

		Vector3 mProjection = _Memory->ReadBuffer<Vector3>(PtrCache::viewState + 0x940);
		double yUnit = _Memory->ReadBuffer<Vector3>(PtrCache::CameraRotation + 0x48).y; // 0 - 1 vert
		
		cam.Rotation.x = std::asin(yUnit) * 180.0f / M_PI;
		cam.Rotation.y = std::atan2(-mProjection.x, mProjection.y) * 180.0f / M_PI;
		cam.Rotation.z = 0.0;

		float gotFov = static_cast<float>(_Memory->Read<double>(PtrCache::viewState + 0x740));
		float fovRadians = 2.0f * std::atanf(1.0f / gotFov);
		cam.FieldOfView = gotFOV;
		cam.Location = _Memory->ReadBuffer<Vector3>(PtrCache::CameraLocation);
		
		//std::cout << "Def:\t" << gotFov << " : " << "Rad:\t" << fovRadians << " : " << "Deg:\t" << fovRadians * 180.0f / M_PI << std::endl;

		return cam;
	}
	
	/*
	inline auto GetViewAngles() -> Engine::Camera
	{
		Engine::Camera cam;

		Vector3 Direction = _Memory->ReadBuffer<Vector3>(PtrCache::CameraRotation);
		double yUnit = _Memory->ReadBuffer<Vector3>(PtrCache::CameraRotation + 0x48).y;

		D3DXVECTOR3 forward((float)Direction.x, (float)yUnit, (float)Direction.z);
		D3DXVec3Normalize(&forward, &forward);

		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

		D3DXVECTOR3 right;
		D3DXVec3Cross(&right, &up, &forward);
		D3DXVec3Normalize(&right, &right);

		D3DXVECTOR3 trueUp;
		D3DXVec3Cross(&trueUp, &forward, &right);
		D3DXVec3Normalize(&trueUp, &trueUp);

		// Build view basis
		D3DXMATRIX rotation;
		rotation._11 = right.x;   rotation._12 = right.y;   rotation._13 = right.z;   rotation._14 = 0.0f;
		rotation._21 = trueUp.x;  rotation._22 = trueUp.y;  rotation._23 = trueUp.z;  rotation._24 = 0.0f;
		rotation._31 = forward.x; rotation._32 = forward.y; rotation._33 = forward.z; rotation._34 = 0.0f;
		rotation._41 = 0.0f;      rotation._42 = 0.0f;      rotation._43 = 0.0f;      rotation._44 = 1.0f;

		D3DXQUATERNION quat;
		D3DXQuaternionRotationMatrix(&quat, &rotation);

		cam.Rotation.x = atan2(2.0 * (quat.w * quat.x + quat.y * quat.z),
			1.0 - 2.0 * (quat.x * quat.x + quat.y * quat.y)) * (180.0 / M_PI);

		cam.Rotation.y = asin(2.0 * (quat.w * quat.y - quat.z * quat.x)) * (180.0 / M_PI);

		cam.Rotation.z = atan2(2.0 * (quat.w * quat.z + quat.x * quat.y),
			1.0 - 2.0 * (quat.y * quat.y + quat.z * quat.z)) * (180.0 / M_PI);

		util::PrintV3("Rotation:\t", cam.Rotation);
		std::cout << std::endl;

		cam.Location = _Memory->ReadBuffer<Vector3>(PtrCache::CameraLocation);
		cam.FieldOfView = static_cast<float>(gotFOV);

		return cam;
	}
	*/

	inline auto ProjectWorldToScreen(Vector3 WorldLocation, Engine::Camera* vCamera) -> Vector2
	{
		D3DMATRIX tempMatrix = Engine::Matrix(vCamera->Rotation, Vector3(0,0,0));

		Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
		Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
		Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

		Vector3 vDelta = WorldLocation - vCamera->Location;
		Vector3 vTransformed = Vector3(
			vDelta.Dot(vAxisY),
			vDelta.Dot(vAxisZ),
			max(vDelta.Dot(vAxisX), 1.0f)
		);

		return Vector2(
			CenterScreen.x + vTransformed.x * (CenterScreen.x / tanf(vCamera->FieldOfView * (float)M_PI / 360.f)) / vTransformed.z,
			CenterScreen.y - vTransformed.y * (CenterScreen.x / tanf(vCamera->FieldOfView * (float)M_PI / 360.f)) / vTransformed.z
		);
	}

	inline auto GetDistanceFromLocalPlayer(Vector3 EntityPosition) -> double
	{
		Vector3 Position = PtrCache::vCamera->Location;
		double dx = EntityPosition.x - Position.x;
		double dy = EntityPosition.y - Position.y;
		double dz = EntityPosition.z - Position.z;
		return std::sqrt(dx * dx + dy * dy + dz * dz);
	}

	inline auto ClearCache() -> void
	{
		PtrCache::Gworld = 0;
		PtrCache::GameInstance = 0;
		PtrCache::Player = 0;
		PtrCache::Mesh = 0;
		PtrCache::PlayerState = 0;
		PtrCache::RootComponent = 0;
		PtrCache::LocalPlayers = 0;
		PtrCache::PlayerController = 0;
		PtrCache::CameraLocation = 0;
		PtrCache::CameraRotation = 0;
		PtrCache::vCamera = nullptr;
	}

	inline auto RefreshCache(std::shared_ptr<driver::c_Memory> Memory_ptr) -> void
	{
		_Memory = Memory_ptr;

		while (true)
		{
			// https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=UWorld
			PtrCache::Gworld = _Memory->Read<uint64_t>(_Memory->currentProcess.base_address + Offsets::UWorld);

			// https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=UWorld&member=OwningGameInstance
			PtrCache::GameInstance = _Memory->Read<uint64_t>(PtrCache::Gworld + Offsets::OwningGameInstance);

			// https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=UGameInstance&member=LocalPlayers
			PtrCache::LocalPlayers = _Memory->Read<uint64_t>(_Memory->Read<uint64_t>(PtrCache::GameInstance + Offsets::LocalPlayers));

			// https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=APlayerController
			PtrCache::PlayerController = _Memory->Read<uint64_t>(PtrCache::LocalPlayers + Offsets::PlayerController);

			// https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=APlayerController&member=AcknowledgedPawn
			PtrCache::Player = _Memory->Read<uint64_t>(PtrCache::PlayerController + Offsets::AcknowledgedPawn);

			// https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AActor&member=RootComponent
			PtrCache::RootComponent = _Memory->Read<uint64_t>(PtrCache::Player + Offsets::RootComponent);

			PtrCache::CameraLocation = _Memory->Read<uintptr_t>(PtrCache::Gworld + 0x178);

			PtrCache::CameraRotation = _Memory->Read<uintptr_t>(PtrCache::Gworld + 0x188);

			PtrCache::viewMatrix = _Memory->Read<uintptr_t>(PtrCache::LocalPlayers + 0xD0);

			PtrCache::viewState = _Memory->Read<uintptr_t>(PtrCache::viewMatrix + 0x8);

			Engine::Camera angles = sdk::GetViewAngles();

			PtrCache::vCamera = &angles;

			Sleep(5);
		}

		ClearCache();
	}
}
