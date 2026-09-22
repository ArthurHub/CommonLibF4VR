#pragma once

#include "RE/NetImmerse/NiLight.h"

namespace RE
{
	// The NiLight of every TESObjectLIGH light, spot lights included: the BSLight gives a spot its cone.
	class __declspec(novtable) NiPointLight :
		public NiLight  // 000
	{
	public:
		static constexpr auto RTTI{ RTTI::NiPointLight };
		static constexpr auto VTABLE{ VTABLE::NiPointLight };
		static constexpr auto Ni_RTTI{ Ni_RTTI::NiPointLight };

		virtual ~NiPointLight();  // 00

		// members - how the light fades over its radius: BSLight::GetLuminanceAtPoint attenuates by
		// attenConstant + attenScalar * pow(distance / radius, attenExponent). TESObjectLIGH::GenDynamic copies the
		// form's values. Offsets are VR's less 0x40 (see NiLight), read from Fallout4VR.exe: the constructor
		// 0x1C27E80 (0, 1, 2), CopyMembers 0x1C27B90; GenDynamic allocates 0x1D0. Flat is unverified.
		float        attenConstant;  // 170
		float        attenScalar;    // 174
		float        attenExponent;  // 178
		std::uint8_t unk17C[0x14];   // 17C
	};
#if defined(ENABLE_FALLOUT_VR) && !defined(ENABLE_FALLOUT_NG) && !defined(ENABLE_FALLOUT_F4)
	static_assert(offsetof(NiPointLight, attenConstant) == 0x1B0);
	static_assert(sizeof(NiPointLight) == 0x1D0);
#else
	static_assert(sizeof(NiPointLight) == 0x190);
#endif
}
