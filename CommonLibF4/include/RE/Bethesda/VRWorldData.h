#pragma once

#include "RE/NetImmerse/NiMatrix3.h"

#ifdef ENABLE_FALLOUT_VR
namespace RE
{
	// The VR world (room) state; other VR mods call it vrDataStruct. The game turns the player by rotating this
	// room, not the actor, whose heading follows the HMD. Fallout 4 VR only, no IDs: read off the turn handler (VR
	// 0x140FCF5E0) of Fallout4VR.exe 1.2.72. Its layout past the room rotation is not mapped.
	class VRWorldData
	{
	public:
		[[nodiscard]] static VRWorldData* GetSingleton()
		{
			static REL::Relocation<VRWorldData**> singleton{ REL::VariantOffset(0, 0, 0x59429C0) };  // VR: tested
			return singleton.address() ? *singleton : nullptr;
		}

		// The room rotation, a SIMD-padded 3x3 at +0x210.
		[[nodiscard]] NiMatrix3& GetRoomRotation() noexcept
		{
			return *reinterpret_cast<NiMatrix3*>(reinterpret_cast<std::uintptr_t>(this) + 0x210);
		}

		// The yaw the game turns the player with, in radians: the engine's own NiMatrix3::ToEulerAnglesZXY of the
		// room rotation, the decomposition its smooth turn pairs with SetYaw, so get, add, set keeps the sign
		// convention. (The snap path uses another decomposition, VR 0x141C0FED0, with a different convention.)
		[[nodiscard]] float GetYaw()
		{
			using func_t = bool (*)(const NiMatrix3*, float*, float*, float*);
			static REL::Relocation<func_t> func{ REL::RelocationID(606387, 0) };  // VR: tested
			float                          yaw = 0.0f;
			float                          b = 0.0f;
			float                          c = 0.0f;
			func(&GetRoomRotation(), &yaw, &b, &c);
			return yaw;
		}

		// Rebuilds the room rotation from one yaw in radians and writes it back: what the engine's smooth turn and
		// its instant snap both call to move the player.
		void SetYaw(float a_yaw)
		{
			using func_t = void (*)(VRWorldData*, const float*);
			static REL::Relocation<func_t> func{ REL::VariantOffset(0, 0, 0x1BA7780) };  // VR: tested
			func(this, &a_yaw);
		}
	};
}
#endif
