#pragma once

#include "offsets.hpp"
#include "../features/util/util.hpp"

#include "../config.h"

#include <memory>
#include <cstdint>
#include <d3dx9math.h>
#include <numbers>

namespace PtrCache
{
	// base classes
	inline uintptr_t Gworld = 0;
	inline uintptr_t GameInstance = 0;
	inline uintptr_t Player = 0;
	inline uintptr_t Mesh = 0;
	inline uintptr_t PlayerState = 0;
	inline uintptr_t RootComponent = 0;
	inline uintptr_t LocalPlayers = 0;
	inline uintptr_t PlayerController = 0;
	
	// used in playercache / esp
	inline uintptr_t GameState = 0;
	inline uintptr_t PlayerArray = 0;
	inline int PlayerList = 0;

	// used in isvisible
	inline double LastRenderTime = FLT_MAX;

	// stored structs
	inline Engine::tarray<uintptr_t> viewMatrix{};
	inline Engine::Camera vCamera{};
	inline Engine::EntityCache Entities{};
	inline Engine::Entity Target{};
}

// precalculate
inline std::shared_ptr<driver::c_Memory> _Memory = nullptr;

namespace sdk
{
	inline static Engine::BoneCache boneCache{};

	inline auto isVisible(uintptr_t Mesh) -> bool
	{ 
		float MeshRenderTime = _Memory->ReadBuffer<float>(Mesh + 0x32C); // UPrimitiveComponent::LastRenderTimeOnScreen
		return (PtrCache::LastRenderTime - MeshRenderTime <= 0.06f);
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
	
	inline auto GetViewAngles() -> Engine::Camera
	{
		Engine::Camera cam;
		
		const Engine::tarray<uintptr_t> ViewStates = PtrCache::viewMatrix;
		if (!ViewStates.is_valid())
			return {};

		const uintptr_t View = ViewStates.get(_Memory, 1);
		if (!View)
			return {};

		const double fovAxis = _Memory->ReadBuffer<double>(View + 0x740);
		const Engine::fmatrix Projection = _Memory->ReadBuffer<Engine::fmatrix>(View + 0x940);
		
		cam.Rotation.x = std::asin(max(-1.0f, min(1.0f, Projection.z_plane.w))) * 180.0f / M_PI;
		cam.Rotation.y = std::atan2(Projection.y_plane.w, Projection.x_plane.w) * 180.0f / M_PI;
		cam.Rotation.z = 0.0;

		//cam.Location = _Memory->ReadBuffer<Vector3>(PtrCache::CameraLocation);
		cam.Location.x = Projection.m[3][0];
		cam.Location.y = Projection.m[3][1];
		cam.Location.z = Projection.m[3][2];

		const float fovRadians = 2.0f * std::atanf(1.0f / static_cast<float>(fovAxis));
		cam.FieldOfView = fovRadians * 180.0f / M_PI;
		
		return cam;
	}
	
	inline auto ProjectWorldToScreen(Vector3 WorldLocation, Engine::Camera vCamera) -> Vector2
	{
		D3DMATRIX tempMatrix = Engine::Matrix(vCamera.Rotation, Vector3(0,0,0));

		Vector3 vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
		Vector3 vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
		Vector3 vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

		Vector3 vDelta = WorldLocation - vCamera.Location;
		Vector3 vTransformed = Vector3(
			vDelta.Dot(vAxisY),
			vDelta.Dot(vAxisZ),
			max(vDelta.Dot(vAxisX), 1.0f)
		);

		return Vector2(
			CenterScreen.x + vTransformed.x * (CenterScreen.x / tanf(vCamera.FieldOfView * (float)M_PI / 360.f)) / vTransformed.z,
			CenterScreen.y - vTransformed.y * (CenterScreen.x / tanf(vCamera.FieldOfView * (float)M_PI / 360.f)) / vTransformed.z
		);
	}

	inline auto GetDistanceFromLocalPlayer(Vector3 EntityPosition) -> double
	{
		Vector3 Position = PtrCache::vCamera.Location;
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
		PtrCache::GameState = 0;
		PtrCache::PlayerArray = 0;
		PtrCache::PlayerList = 0;
		PtrCache::viewMatrix = {};
		PtrCache::vCamera = {};
		PtrCache::Entities = {};
		PtrCache::Target = {};
		PtrCache::LastRenderTime = FLT_MAX;
	}

	inline auto RefreshCache(std::weak_ptr<driver::c_Memory> _memory) -> void
	{
		if (!(_Memory = _memory.lock()))
			return;

		while (_Memory)
		{
			// https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=UWorld
			PtrCache::Gworld = _Memory->Read<uintptr_t>(_Memory->currentProcess.base_address + Offsets::UWorld);

			// https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=UWorld&member=OwningGameInstance
			PtrCache::GameInstance = _Memory->Read<uintptr_t>(PtrCache::Gworld + Offsets::OwningGameInstance);

			// https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=UGameInstance&member=LocalPlayers
			PtrCache::LocalPlayers = _Memory->Read<uintptr_t>(_Memory->Read<uintptr_t>(PtrCache::GameInstance + Offsets::LocalPlayers));

			// https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=APlayerController
			PtrCache::PlayerController = _Memory->Read<uintptr_t>(PtrCache::LocalPlayers + Offsets::PlayerController);

			// https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=APlayerController&member=AcknowledgedPawn
			PtrCache::Player = _Memory->Read<uintptr_t>(PtrCache::PlayerController + Offsets::AcknowledgedPawn);

			// https://dumpspace.spuckwaffel.com/Games/?hash=6b77eceb&type=classes&idx=AActor&member=RootComponent
			PtrCache::RootComponent = _Memory->Read<uintptr_t>(PtrCache::Player + Offsets::RootComponent);

			PtrCache::GameState = _Memory->Read<uintptr_t>(PtrCache::Gworld + Offsets::GameState);
			
			PtrCache::PlayerArray = _Memory->Read<uintptr_t>(PtrCache::GameState + Offsets::PlayerArray);
			
			PtrCache::PlayerList = _Memory->ReadBuffer<int>(PtrCache::GameState + (Offsets::PlayerArray + sizeof(uintptr_t)));

			PtrCache::LastRenderTime = _Memory->ReadBuffer<double>(PtrCache::Gworld + 0x198); // UWorld::LastRenderTime

			PtrCache::viewMatrix = _Memory->ReadBuffer<Engine::tarray<uintptr_t>>(PtrCache::LocalPlayers + 0xD0);

			PtrCache::vCamera = sdk::GetViewAngles();

			Sleep(5);
		}

		ClearCache();
	}
}
