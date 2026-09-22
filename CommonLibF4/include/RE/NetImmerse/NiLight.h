#pragma once

#include "RE/NetImmerse/NiAVObject.h"
#include "RE/NetImmerse/NiBound.h"
#include "RE/NetImmerse/NiColor.h"
#include "RE/NetImmerse/NiPoint.h"

namespace RE
{
	class __declspec(novtable) NiLight :
		public NiAVObject  // 000
	{
	public:
		static constexpr auto RTTI{ RTTI::NiLight };
		static constexpr auto VTABLE{ VTABLE::NiLight };
		static constexpr auto Ni_RTTI{ Ni_RTTI::NiLight };

		virtual ~NiLight();  // 00

		// members - flat offsets as in libxse-commonlibf4 (NG). VR has each 0x40 higher, after the 0x40 bytes VR
		// adds to NiAVObject: read from Fallout4VR.exe, the constructor 0x1C362A0, CopyMembers 0x1C35F90,
		// TESObjectLIGH::GenDynamic 0x306A50 (fills a new light from the form) and BSLight::GetLuminanceAtPoint
		// 0x286EA60 (reads diffuse, radius and dimmer on every call).
#if defined(ENABLE_FALLOUT_VR) && !defined(ENABLE_FALLOUT_NG) && !defined(ENABLE_FALLOUT_F4)
		std::uint32_t pad120[0x10];  // 120
#endif
		NiColor  ambient;                  // 120
		NiColor  diffuse;                  // 12C - the form's color / 255, negated for TES_LIGHT_FLAGS::kNegative
		NiPoint3 radius;                   // 138 - NetImmerse's specular color, which holds the light radius in all three
		float    dimmer;                   // 144 - the form's fade
		alignas(0x10) NiBound modelBound;  // 150 - the bound of the BSLight's light volume
		void*        rendererData;         // 160
		std::uint8_t unk168;               // 168
	};
#if defined(ENABLE_FALLOUT_VR) && !defined(ENABLE_FALLOUT_NG) && !defined(ENABLE_FALLOUT_F4)
	static_assert(offsetof(NiLight, ambient) == 0x160);
	static_assert(offsetof(NiLight, dimmer) == 0x184);
	static_assert(offsetof(NiLight, modelBound) == 0x190);
	static_assert(sizeof(NiLight) == 0x1B0);
#else
	static_assert(sizeof(NiLight) == 0x170);
#endif
}
