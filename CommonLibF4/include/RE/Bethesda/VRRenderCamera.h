#pragma once

#ifdef ENABLE_FALLOUT_VR
// The per-eye camera the engine rendered the current frame with: each eye's view-projection matrix and the
// world-to-VR-origin offset (posAdjust), HMD pose included. Drawing with these puts geometry exactly where the game
// drew. Fallout 4 VR only, no IDs, Fallout4VR.exe 1.2.72; if drawn geometry reads as garbage after a runtime change,
// this block is the first suspect.
namespace RE::VRRenderCamera
{
	namespace detail
	{
		inline constexpr std::size_t CAMERA_DATA = 0x25D0;  // -> the camera data block
		inline constexpr std::size_t EYE_VIEW_PROJ[2]{ 0xD0, 0x2E0 };
		// The engine copies 2590 / 25A0 / 25B0 / 25C0 as one family: 25B0 and 25C0 hold the previous frame's
		// origins, for temporal reprojection, and reading 25C0 as the right eye lags one frame of room translation.
		inline constexpr std::size_t EYE_POS_ADJUST[2]{ 0x2590, 0x25A0 };
		static_assert(EYE_POS_ADJUST[1] - EYE_POS_ADJUST[0] == 0x10);

		[[nodiscard]] inline std::uintptr_t Globals()
		{
			const auto address = REL::VariantOffset(0, 0, 0x6235AC8).address();  // VR: tested
			return address ? *reinterpret_cast<std::uintptr_t*>(address) : 0;    // NOLINT(performance-no-int-to-ptr)
		}
	}

	// The eye's (0 left, 1 right) view-projection matrix, column-major 4x4; null before the renderer is up.
	[[nodiscard]] inline const float* GetEyeViewProjection(std::uint32_t a_eye)
	{
		const auto globals = detail::Globals();
		const auto data = globals && a_eye < 2 ? *reinterpret_cast<std::uintptr_t*>(globals + detail::CAMERA_DATA) : 0;  // NOLINT(performance-no-int-to-ptr)
		return data ? reinterpret_cast<const float*>(data + detail::EYE_VIEW_PROJ[a_eye]) : nullptr;                     // NOLINT(performance-no-int-to-ptr)
	}

	// The eye's posAdjust (x, y, z) for the current frame; null before the renderer is up.
	[[nodiscard]] inline const float* GetEyePosAdjust(std::uint32_t a_eye)
	{
		const auto globals = detail::Globals();
		return globals && a_eye < 2 ? reinterpret_cast<const float*>(globals + detail::EYE_POS_ADJUST[a_eye]) : nullptr;  // NOLINT(performance-no-int-to-ptr)
	}
}
#endif
