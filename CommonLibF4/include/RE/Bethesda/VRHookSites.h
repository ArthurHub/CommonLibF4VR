#pragma once

#ifdef ENABLE_FALLOUT_VR
// Places in Fallout4VR.exe 1.2.72 that mods redirect or patch, each with what the unpatched game has there, so a
// mod can check it before writing and leave the game alone when the bytes differ (another runtime, or another mod
// already there). Fallout 4 VR only, no IDs.
namespace RE::VRHookSites
{
	// A `call rel32` (E8, 5 bytes) a mod redirects with write_call<5>, and the function it calls.
	struct CallSite
	{
	public:
		[[nodiscard]] std::uintptr_t address() const { return REL::VariantOffset(0, 0, site).address(); }
		[[nodiscard]] std::uintptr_t callee() const { return REL::VariantOffset(0, 0, target).address(); }

		// Whether the site is still a call to the callee: false off VR, or once something redirected it.
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
		std::size_t site;    // the call instruction
		std::size_t target;  // the function it calls
	};

	// Bytes a mod overwrites, and what the unpatched game has there.
	struct PatchSite
	{
	public:
		[[nodiscard]] std::uintptr_t address() const { return REL::VariantOffset(0, 0, site).address(); }

		// Whether the bytes are still the game's own: false off VR, or once something patched them.
		[[nodiscard]] bool Verify() const
		{
			const auto at = address();
			return at && std::memcmp(reinterpret_cast<const void*>(at), expected.data(), expected.size()) == 0;  // NOLINT(performance-no-int-to-ptr)
		}

		// members
		std::size_t                   site;
		std::span<const std::uint8_t> expected;
	};

	namespace detail
	{
		inline constexpr std::uint8_t PLAYER_POST_ANIM_GRAPH[]{ 0xF6, 0x81, 0x56, 0x12, 0x00, 0x00, 0x10 };
		inline constexpr std::uint8_t PLAYER_UPDATE_VCALL_458[]{ 0xFF, 0x90, 0x58, 0x04, 0x00, 0x00 };
		inline constexpr std::uint8_t PRIMARY_WAND_MESH[]{ 'D', 'a', 't', 'a', '\\', 'M', 'e', 's', 'h', 'e', 's', '\\', 'w', 'o', 'r', 'l', 'd', '_', 'p', 'r', 'i', 'm', 'a', 'r', 'y', 'W', 'a', 'n', 'd', '.', 'n', 'i', 'f', 0 };
	}

	// Main loop, Main::OnIdle and the update it runs. VR: checked, unless marked.

	// Once a frame on the main thread, a call to a small lock helper; F4VR-CommonFramework's frame tick.  // VR: tested
	inline constexpr CallSite MainUpdateFrameTick{ 0xD8405E, 0xD3C820 };
	// Main::OnIdle's call of the VR locomotion step; FRIK's smooth movement.  // VR: tested
	inline constexpr CallSite OnIdleVRMovement{ 0xD83EC4, 0x1BA7BA0 };
	inline constexpr CallSite MainUpdateSwap{ 0xD844BC, 0xD831F0 };  // Main::Swap
	inline constexpr CallSite MainUpdateMultiBoundCulling{ 0xD8445D, 0xD84930 };
	inline constexpr CallSite MainUpdateBoneTreeUpdate{ 0xD84EE4, 0xF10ED0 };
	inline constexpr CallSite MainUpdateEnd{ 0xD84F2C, 0xDA09A0 };
	inline constexpr CallSite DrawWorldAndUIDraw{ 0xD87ACE, 0x2841530 };  // in Main::DrawWorld_And_UI

	// The player.

	// The player update's NiAVObject::Update of the player's 3D.  // VR: tested
	inline constexpr CallSite PlayerUpdate3D{ 0xF0FF6A, 0x1C22FB0 };
	// The player update's call of TESObjectREFR::SetupAnimationUpdateDataForReference, with the frame's delta time.  // VR: tested
	inline constexpr CallSite PlayerAnimationUpdateData{ 0xF0FBDF, 0x4189C0 };
	// PlayerCharacter::Update1stPersonArm, called twice a frame.  // VR: tested
	inline constexpr CallSite Update1stPersonArm1{ 0xEF6108, 0xEF6280 };
	inline constexpr CallSite Update1stPersonArm2{ 0xEF614B, 0xEF6280 };
	// Actor::ReEquipAll when leaving power armor.  // VR: tested
	inline constexpr CallSite PowerArmorExitReEquipAll{ 0xF01528, 0xDDF050 };
	// ExtraDataList::SetMultiBoundRef when entering power armor.  // VR: tested
	inline constexpr CallSite PowerArmorEnterSetMultiBoundRef{ 0xF00DC6, 0x91320 };
	inline constexpr CallSite GunReloadGetCurrentWeapon{ 0xF3027C, 0xE50DA0 };  // Actor::GetCurrentWeapon
	// BSFlattenedBoneTree's NiNode::UpdateDownwardPass, right before it refreshes its own world transforms.
	inline constexpr CallSite BoneTreeUpdateDownwardPass{ 0x1C21156, 0x1C18620 };
	// PipboyManager::InitPipboy's PlayerCharacter::IsPipboyLightOn as the Pip-Boy opens: it saves the result as
	// wasPipboyLightActive and hides the light when set, and the close shows it again. In power armor it stores
	// false without the call, which is why the light stays on there; an `xor eax, eax` in its place does that
	// everywhere.  // VR: tested
	inline constexpr CallSite InitPipboyIsPipboyLightOn{ 0xC34AB1, 0xF27790 };

	// Rendering.  // VR: tested

	// The scene depth-stencil is bound while this commits the graphics state.
	inline constexpr CallSite SceneDepthCommitGraphicsState{ 0x1D8E84A, 0x1D9B5D0 };
	// The player-space pass draw: the Pip-Boy, then PlayerWorldNode (hands, wands).
	inline constexpr CallSite PlayerPassDraw{ 0x284E68C, 0x27FF820 };

	// Patch sites.

	// The body of a PlayerCharacter virtual (at 0xF2F0A0) that, after the animation graph update, resets the body
	// pose to hang off the camera; FRIK replaces it.  // VR: tested
	inline constexpr PatchSite PlayerPostAnimGraph{ 0xF2F0A8, detail::PLAYER_POST_ANIM_GRAPH };
	// The player update's virtual call (call [rax+458h], 6 bytes).
	inline constexpr PatchSite PlayerUpdateVirtualCall458{ 0xF1004C, detail::PLAYER_UPDATE_VCALL_458 };
	// The primary wand's mesh path, a string the game loads it from.  // VR: tested
	inline constexpr PatchSite PrimaryWandMeshPath{ 0x2D686D8, detail::PRIMARY_WAND_MESH };
}
#endif
