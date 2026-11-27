#pragma once

#if defined(ENABLE_FALLOUT_F4) || defined(ENABLE_FALLOUT_NG)
// Places in flat Fallout 4 that mods redirect, each with what the unpatched game has there, so a mod can check it
// before writing and leave the game alone when the bytes differ (another runtime, or another mod already there).
// The flat counterpart of VRHookSites: a site is a function's ID and an offset into it per runtime. Nothing on VR.
namespace RE::FlatHookSites
{
	// A `call rel32` (E8, 5 bytes) at an offset into a function, which a mod redirects with write_call<5>, and the
	// function it calls.
	struct CallSite
	{
	public:
		[[nodiscard]] std::uintptr_t address() const
		{
			if (REL::Module::IsVR()) {
				return 0;
			}
			const auto start = function.address();
			return start ? start + offset.offset() : 0;
		}

		[[nodiscard]] std::uintptr_t callee() const { return REL::Module::IsVR() ? 0 : target.address(); }

		// Whether the site is still a call to the callee: false on VR, or once something redirected it.
		[[nodiscard]] bool Verify() const
		{
			const auto at = address();
			if (!at || !callee()) {
				return false;
			}
			const auto   bytes = reinterpret_cast<const std::uint8_t*>(at);  // NOLINT(performance-no-int-to-ptr)
			std::int32_t rel = 0;
			std::memcpy(&rel, bytes + 1, sizeof(rel));
			return bytes[0] == 0xE8 && at + 5 + rel == callee();
		}

		// members
		REL::RelocationID  function;  // the function the call is in
		REL::VariantOffset offset;    // the call instruction's offset into it
		REL::RelocationID  target;    // the function it calls
	};

	// Main loop.

	// Main::OnIdle's call of BGSDynamicPersistenceManager::DemoteQueuedActors, behind the same game-data-loaded gate as
	// VR's MainUpdateFrameTick: once a frame on the main thread, in menus and while paused, before the frame is drawn.
	// F4VR-CommonFramework's frame tick on flat.
	inline constexpr CallSite MainUpdateFrameTick{
		REL::RelocationID(633524, 2228917),  // Main::OnIdle
		REL::VariantOffset(0x210, 0x492, 0),
		REL::RelocationID(727554, 2193821)  // BGSDynamicPersistenceManager::DemoteQueuedActors
	};
}
#endif
