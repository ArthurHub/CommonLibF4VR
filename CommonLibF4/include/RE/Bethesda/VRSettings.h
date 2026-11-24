#pragma once

#include "RE/Bethesda/Settings.h"

#ifdef ENABLE_FALLOUT_VR
// The Fallout 4 VR settings the game keeps in Fallout4Prefs.ini [VR] and Fallout4.ini [VRInput], read straight off
// the game's Setting objects: a name lookup in INIPrefSettingCollection costs far more per frame. Fallout 4 VR only,
// no IDs; each Setting was read out of Fallout4VR.exe 1.2.72 by its vtable and name. Every accessor returns
// nullptr or the default off VR.
namespace RE::VRSettings
{
	namespace detail
	{
		[[nodiscard]] inline Setting* At(std::size_t a_vrOffset)
		{
			const auto address = REL::VariantOffset(0, 0, a_vrOffset).address();
			return address ? reinterpret_cast<Setting*>(address) : nullptr;  // NOLINT(performance-no-int-to-ptr)
		}
	}

	// Where the game puts the Pip-Boy, in the order of the game's settings menu.
	enum class PipboyPlacement : std::uint8_t
	{
		kWrist = 0,
		kInFront = 1,
		kProjected = 2,
	};

	// The turning style picked in the VR comfort settings (iRotationType:VR).
	enum class RotationType : std::uint8_t
	{
		kNone = 0,          // turning is off
		kSnapSmoothed = 1,  // snap by fRotationAngle, interpolated there at fAngleSnapSmoothingSpeed
		kSnapInstant = 2,   // snap in one frame (the game also plays a comfort fade)
		kSmooth = 3,        // continuous turn at fRotationSpeed while the stick is held
	};

	[[nodiscard]] inline Setting* LeftHandedMode() { return detail::At(0x37D5E40); }              // bLeftHandedMode:VR  // VR: tested
	[[nodiscard]] inline Setting* AlwaysUseProjectedPipboy() { return detail::At(0x37B4278); }    // bAlwaysUseProjectedPipboy:VR  // VR: tested
	[[nodiscard]] inline Setting* AttachPipboyToHMD() { return detail::At(0x37B4290); }           // bAttachPipboyToHMD:VR  // VR: tested
	[[nodiscard]] inline Setting* ComfortSneak() { return detail::At(0x37D6170); }                // bComfortSneak:VR  // VR: tested
	[[nodiscard]] inline Setting* UseWandDirectionalMovement() { return detail::At(0x37D6158); }  // bUseWandDirectionalMovement:VRInput  // VR: tested
	[[nodiscard]] inline Setting* RotationTypeSetting() { return detail::At(0x37D5E70); }         // iRotationType:VR  // VR: tested
	[[nodiscard]] inline Setting* RotationAngle() { return detail::At(0x37D5E88); }               // fRotationAngle:VR, degrees  // VR: tested
	[[nodiscard]] inline Setting* RotationSpeed() { return detail::At(0x37D5EA0); }               // fRotationSpeed:VR, degrees a second  // VR: tested
	[[nodiscard]] inline Setting* AngleSnapSmoothingSpeed() { return detail::At(0x37CFD90); }     // fAngleSnapSmoothingSpeed:VR, degrees a second  // VR: tested

	[[nodiscard]] inline bool IsLeftHandedMode()
	{
		const auto setting = LeftHandedMode();
		return setting && setting->GetBinary();
	}

	[[nodiscard]] inline bool IsComfortSneak()
	{
		const auto setting = ComfortSneak();
		return setting && setting->GetBinary();
	}

	[[nodiscard]] inline bool IsWandDirectionalMovement()
	{
		const auto setting = UseWandDirectionalMovement();
		return setting && setting->GetBinary();
	}

	// In front (bAttachPipboyToHMD) wins over projected; neither is the wrist.
	[[nodiscard]] inline PipboyPlacement GetPipboyPlacement()
	{
		const auto inFront = AttachPipboyToHMD();
		const auto projected = AlwaysUseProjectedPipboy();
		if (inFront && inFront->GetBinary()) {
			return PipboyPlacement::kInFront;
		}
		return projected && projected->GetBinary() ? PipboyPlacement::kProjected : PipboyPlacement::kWrist;
	}

	// Sets where the game puts the Pip-Boy as its settings menu does: writes both settings, runs the handlers the
	// game installed on each (SettingHandlerMap::SettingUpdated), and saves Fallout4Prefs.ini. Game thread only.
	inline void SetPipboyPlacement(PipboyPlacement a_placement)
	{
		const auto projected = AlwaysUseProjectedPipboy();
		const auto inFront = AttachPipboyToHMD();
		if (!projected || !inFront) {
			return;
		}
		projected->SetBinary(a_placement == PipboyPlacement::kProjected);
		SettingHandlerMap::SettingUpdated(*projected);
		inFront->SetBinary(a_placement == PipboyPlacement::kInFront);
		SettingHandlerMap::SettingUpdated(*inFront);
		if (const auto prefs = INIPrefSettingCollection::GetSingleton(); prefs && prefs->Open(true)) {
			prefs->WriteSettings();
			prefs->Close();
		}
	}

	[[nodiscard]] inline RotationType GetRotationType()
	{
		const auto setting = RotationTypeSetting();
		return setting ? static_cast<RotationType>(setting->GetInt()) : RotationType::kNone;
	}
}
#endif
