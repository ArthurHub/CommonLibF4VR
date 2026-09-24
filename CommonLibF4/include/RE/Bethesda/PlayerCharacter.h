#pragma once

#include "RE/Bethesda/AITimeStamp.h"
#include "RE/Bethesda/Actor.h"
#include "RE/Bethesda/BGSCharacterTint.h"
#include "RE/Bethesda/BSLock.h"
#include "RE/Bethesda/BSPointerHandle.h"
#include "RE/Bethesda/BSSoundHandle.h"
#include "RE/Bethesda/BSTArray.h"
#include "RE/Bethesda/BSTEvent.h"
#include "RE/Bethesda/BSTHashMap.h"
#include "RE/Bethesda/BSTList.h"
#include "RE/Bethesda/BSTOptional.h"
#include "RE/Bethesda/BSTSmartPointer.h"
#include "RE/Bethesda/BSTTuple.h"
#include "RE/Bethesda/IMovementInterface.h"
#include "RE/Havok/hkRefPtr.h"
#include "RE/NetImmerse/NiPoint.h"
#include "RE/NetImmerse/NiSmartPointer.h"
#include "RE/NetImmerse/NiTMap.h"

namespace RE
{
	enum class DEFAULT_OBJECT;
	enum class DIFFICULTY_LEVEL;
	enum class PLAYER_ACTION;
	enum class QUEST_OBJECTIVE_STATE;
	enum class SCENE_ACTION_PLAYER_RESPONSE_TYPE;

	class BGSQuestObjective;
	class BipedAnim;
	class BSLight;
	class BSPathingSolution;
	class Crime;
	class DialoguePackage;
	class DoorTeleportData;
	class EffectSetting;
	class hknpBSMouseSpringAction;
	class ImageSpaceModifierInstanceDOF;
	class ImageSpaceModifierInstanceForm;
	class MenuModeChangeEvent;
	class MenuOpenCloseEvent;
	class NiAVObject;
	class NiLight;
	class BSTriShape;
	class NiCamera;
	class NiNode;
	class OtherEventEnabledEvent;
	class QuestTargetArray;
	class TeleportPath;
	class TESHitEvent;
	class TESQuestStageItem;
	class UserEventEnabledEvent;

	struct BGSActorCellEvent;
	struct BGSActorDeathEvent;
	struct ItemChange;
	struct PickRefUpdateEvent;
	struct PositionPlayerEvent;

	namespace PerkValueEvents
	{
		struct PerkEntryUpdatedEvent;
	}

	enum class COMMAND_TYPE
	{
		kNone = 0x0,
		kCall = 0x1,
		kFollow = 0x2,
		kMove = 0x3,
		kAttack = 0x4,
		kInspect = 0x5,
		kRetrieve = 0x6,
		kStay = 0x7,
		kRelease = 0x8,
		kHeal = 0x9,
		kAssign = 0xA,
		kRide = 0xB,
		kEnter = 0xC,
	};

	struct CrimeGoldStruct
	{
	public:
		// members
		std::uint32_t violent;                     // 00
		std::uint32_t nonViolent;                  // 04
		std::uint32_t totalNonViolentAccumulated;  // 08
		std::uint32_t totalViolentAccumulated;     // 0C
	};
	static_assert(sizeof(CrimeGoldStruct) == 0x10);

	struct StolenItemValueStruct
	{
	public:
		// members
		std::uint32_t stolenItemValueNoCrime;  // 0
		std::uint32_t istolenItemValueCrime;   // 4
	};
	static_assert(sizeof(StolenItemValueStruct) == 0x8);

	struct PlayerCoverData
	{
	public:
		enum class PEEK_STATE;

		// members
		NiPoint3                               coverLocation;  // 00
		NiPoint3                               peekLocation;   // 0C
		float                                  peekTimer;      // 18
		float                                  updateTimer;    // 2C
		float                                  blockedTimer;   // 20
		REX::EnumSet<PEEK_STATE, std::int32_t> peekState;      // 24
		bool                                   gunBlocked;     // 28
	};
	static_assert(sizeof(PlayerCoverData) == 0x2C);

	struct FriendshipFactionsStruct
	{
	public:
		// members
		std::uint16_t friendCounts[4];  // 0
	};
	static_assert(sizeof(FriendshipFactionsStruct) == 0x8);

	class BGSInstancedQuestObjective
	{
	public:
		// members
		BGSQuestObjective*                                objective;      // 00
		std::uint32_t                                     instanceID;     // 08
		REX::EnumSet<QUEST_OBJECTIVE_STATE, std::int32_t> enstanceState;  // 0C
	};
	static_assert(sizeof(BGSInstancedQuestObjective) == 0x10);

	struct SayOnceTimeStampStruct
	{
	public:
		// members
		std::uint32_t date;  // 0
		float         hour;  // 4
	};
	static_assert(sizeof(SayOnceTimeStampStruct) == 0x8);

	struct PLAYER_TARGET_LOC
	{
	public:
		using ArrivalFunc_t = void(void*);

		// members
		TESWorldSpace*    world;             // 00
		TESObjectCELL*    interior;          // 08
		DoorTeleportData* transitionTel;     // 10
		NiPoint3          location;          // 18
		NiPoint3          angle;             // 24
		TESObjectREFR*    walkThroughDoor;   // 30
		ArrivalFunc_t*    arrivalFunc;       // 38
		void*             arrivalFuncData;   // 40
		ObjectRefHandle   furnitureRef;      // 48
		ObjectRefHandle   fastTravelMarker;  // 4C
		float             fastTravelDist;    // 50
		bool              resetWeather;      // 54
		bool              allowAutoSave;     // 55
		bool              preventLoadMenu;   // 56
		bool              skyTransition;     // 57
		bool              isValid;           // 58
	};
	static_assert(sizeof(PLAYER_TARGET_LOC) == 0x60);

	class PlayerCommandTypeEvent :
		public BSTValueEvent<COMMAND_TYPE>  // 0
	{
	public:
	};
	static_assert(sizeof(PlayerCommandTypeEvent) == 0x8);

	class PlayerAutoAimActorEvent :
		public BSTValueEvent<ActorHandle>  // 0
	{
	public:
	};
	static_assert(sizeof(PlayerAutoAimActorEvent) == 0x8);

	struct XPChangeData
	{
	public:
		// members
		std::uint32_t playerLevel;              // 00
		float         initialExperiencePoints;  // 04
		float         experiencePointsAdded;    // 08
		bool          syncedXP;                 // 0C
	};
	static_assert(sizeof(XPChangeData) == 0x10);

#ifdef ENABLE_FALLOUT_VR
	// f4sevr-port: the VR-only wand / HMD node table PlayerCharacter carries inline at 6E0, which the
	// flat game has no counterpart for. FRIK found it first; F4VR-CommonFramework re-exports it.
	//
	// The names are the engine own ones. Most were read out of Fallout4VR.exe 1.2.72, where the VR node
	// setup at 0xEF21DD creates a node, stores it in its slot and calls SetName with a string literal;
	// the rest are nodes it looks up out of loaded NIFs, which name themselves nowhere in code, so those
	// were read off the live table in game. The quoted name is what the node calls itself at runtime,
	// and the unk slots held null in every state sampled - in and out of power armor, Pip-Boy open,
	// light on and off.
	//
	// Two slots hold the melee offset node currently in effect rather than one of their own: 0xF0A1B0
	// points 758 at primaryMeleeWeaponOffsetNode (730) or at a node derived from the equipped weapon,
	// and 760 the same way from 790, which is why they read as a second node with the same name. In
	// game they hold those nodes while nothing is drawn and go null as soon as a weapon is, melee or
	// not, which is also when equippedWeaponNode fills in.
	//
	// The table proper is 40 ref-counted slots, 6E0 through 818: the ctor nulls exactly that range and
	// the cleanup at 0xEEFE71 releases exactly it. The members after it are lockpicking state, not part
	// of the table, and are never released with it.
	struct VRPlayerNodes
	{
	public:
		// members
		NiNode*     playerWorldNode;                             // 00 (6E0) "PlayerWorldNode"
		NiNode*     roomNode;                                    // 08 (6E8) "RoomNode"
		NiNode*     primaryWandNode;                             // 10 (6F0) "PrimaryWandNode"
		NiNode*     primaryWandTouchpad;                         // 18 (6F8) "PrimaryWandTouchpad"
		NiNode*     primaryUIAttachNode;                         // 20 (700) "PrimaryUIAttachNode"
		NiNode*     primaryWeaponToWandNode;                     // 28 (708) "PrimaryWeaponToWandNode"
		NiNode*     primaryWeaponKickbackRecoilNode;             // 30 (710) "PrimaryWeaponKickbackRecoilNode"
		NiNode*     primaryWeaponOffsetNode;                     // 38 (718) "PrimaryWeaponOffsetNode"
		NiCamera*   primaryWeaponScopeCamera;                    // 40 (720) "PrimaryWeaponScopeCamera"
		NiNode*     primaryVertibirdMinigunOffsetNode;           // 48 (728) "PrimaryVertibirdMinigunOffsetNode"
		NiNode*     primaryMeleeWeaponOffsetNode;                // 50 (730) "PrimaryMeleeWeaponOffsetNode"
		NiNode*     primaryUnarmedPowerArmorWeaponOffsetNode;    // 58 (738) "PrimaryUnarmedPowerArmorWeaponOffsetNode"
		BSTriShape* primaryWandLaserPointer;                     // 60 (740) "PrimaryWandLaserPointer"
		NiNode*     primaryWandLaserPointerAdjuster;             // 68 (748) "PrimaryWandLaserPointerAdjuster"
		NiNode*     equippedWeaponNode;                          // 70 (750) the drawn weapon 3D, null while nothing is drawn
		NiNode*     primaryMeleeWeaponOffsetNodeInUse;           // 78 (758) the one in effect - see the note above
		NiNode*     secondaryMeleeWeaponOffsetNodeInUse;         // 80 (760) the one in effect - see the note above
		NiNode*     secondaryWandNode;                           // 88 (768) "SecondaryWandNode"
		NiNode*     point002Node;                                // 90 (770) "Point002", a BSFadeNode under the Pip-Boy
		NiNode*     workshopPalletNode;                          // 98 (778) "WorkshopPallet"
		NiNode*     workshopPalletSlider;                        // A0 (780) "WorkshopPalletSlider"
		NiNode*     secondaryUIOffsetNode;                       // A8 (788) "SecondaryUIOffsetNode"
		NiNode*     secondaryMeleeWeaponOffsetNode;              // B0 (790) "SecondaryMeleeWeaponOffsetNode"
		NiNode*     secondaryUnarmedPowerArmorWeaponOffsetNode;  // B8 (798) "SecondaryUnarmedPowerArmorWeaponOffsetNode"
		NiNode*     secondaryAimNode;                            // C0 (7A0) "SecondaryAimNode"
		NiNode*     pipboyParentNode;                            // C8 (7A8) "PipboyParent"
		NiNode*     pipboyRootNIFOnlyNode;                       // D0 (7B0) "PipboyRoot_NIF_ONLY"
		NiNode*     screenNode;                                  // D8 (7B8) "Screen", under PipboyBody
		NiNode*     pipboyLightParentNode;                       // E0 (7C0) "PipboyLightParent" - ShowPipboyLight (0xF277B0) reads this one
		NiNode*     unk7C8;                                      // E8 (7C8) always null so far
		NiNode*     scopeParentNode;                             // F0 (7D0) "ScopeParent"
		NiNode*     compassDialsNode;                            // F8 (7D8) "CompassDials" - only set in power armor
		NiNode*     hmdNode;                                     // 100 (7E0) "HMDNode"
		NiNode*     offscreenHmdNode;                            // 108 (7E8) "OffscreenHmdNode"
		NiNode*     uprightHmdNode;                              // 110 (7F0) "UprightHmdNode"
		NiNode*     uprightHmdLagNode;                           // 118 (7F8) "UprightHmdLagNode"
		NiNode*     blackSphereNode;                             // 120 (800) "BlackSphere"
		NiNode*     headLightParentNode;                         // 128 (808) "HeadLightParent" - the head lamp hangs here
		NiNode*     unk810;                                      // 130 (810) always null so far
		NiNode*     weaponLeftNode;                              // 138 (818) "WeaponLeft", in the player skeleton under LArm_Hand - last ref-counted slot

		// Lockpicking, set up right after the table by the same function: 0x141D0DD80 loads each model,
		// then a fresh NiNode (0x180 bytes, the VR size) becomes their parent. The two models read as
		// something that is not a game object, so they are left untyped.
		std::uint32_t unk820;              // 140 (820) a handle the ctor seeds from a global
		std::uint32_t pad824;              // 144 (824)
		NiNode*       lockPickParentNode;  // 148 (828) unnamed at runtime, under "projectedMenu"
		void*         lockPickLockModel;   // 150 (830) Interface/Lockpicking/LockPickLock01.nif
		void*         lockPickModel;       // 158 (838) Interface/Lockpicking/LockPick01.nif
	};
	static_assert(sizeof(VRPlayerNodes) == 0x160);
	static_assert(offsetof(VRPlayerNodes, primaryUIAttachNode) == 0x700 - 0x6E0);
	static_assert(offsetof(VRPlayerNodes, pipboyLightParentNode) == 0x7C0 - 0x6E0);
	static_assert(offsetof(VRPlayerNodes, hmdNode) == 0x7E0 - 0x6E0);
	static_assert(offsetof(VRPlayerNodes, headLightParentNode) == 0x808 - 0x6E0);
	static_assert(offsetof(VRPlayerNodes, lockPickParentNode) == 0x828 - 0x6E0);
#endif
#ifdef ENABLE_FALLOUT_VR
	// f4sevr-port: the VR-only run between the node table and PlayerCharacter's first flat member.
	// Mapped from the constructor at 0xEED4F0 (which fixes the field boundaries and widths), from
	// counting which engine code touches each offset, and from logging every field that changed per
	// frame in game while doing one thing at a time - which is what named most of what is named here. Named where there is evidence; everything else keeps its offset as its
	// name, with the width the constructor implies, so the layout stays checkable.
	struct VRPlayerState
	{
	public:
		// members
		std::uint8_t  unk840;             // 00 (840)
		std::byte     pad841[0x3];        // 01 (841)
		float         unk844;             // 04 (844)
		float         unk848;             // 08 (848)
		float         unk84C;             // 0C (84C)
		float         unk850;             // 10 (850)  ctor seeds 25.0
		std::uint8_t  unk854;             // 14 (854)  1 while the Pip-Boy was open, cleared on its close and not set again since
		std::byte     pad855[0x3];        // 15 (855)
		std::byte     unk858[0x34];       // 18 (858)  a list: the ctor points +8 back at itself, and +30 held 0x30 in game
		std::byte     pad88C[0x4];        // 4C (88C)
		float         uiAnchorYaw;        // 50 (890)  the world-space UI anchor yaw (F4VROffsets.h)
		float         uiHeadYaw;          // 54 (894)  the head yaw it trails
		void*         unk898;             // 58 (898)
		void*         unk8A0;             // 60 (8A0)
		float         unk8A8;             // 68 (8A8)  game-time seconds, stamped on the frame actionCooldown below is armed: two stamps 40.9 s apart matched the wall clock exactly
		std::uint32_t unk8AC;             // 6C (8AC)
		std::uint32_t unk8B0;             // 70 (8B0)
		NiPoint3      unk8B4;             // 74 (8B4)
		std::uint8_t  unk8C0[8];          // 80 (8C0)  packed: the ctor writes a word, a byte, then an unaligned word. [0] goes 1 with unk8F8 on entering power armor
		float         snapTurnTargetYaw;  // 88 (8C8)  the snap-turn target (F4VROffsets.h)
		std::uint32_t rightHandedMode;    // 8C (8CC)  1 when bLeftHandedMode:VR is off
		std::uint32_t leftHandedMode;     // 90 (8D0)  1 when it is on
		std::uint32_t unk8D4;             // 94 (8D4)
		void*         unk8D8;             // 98 (8D8)
		void*         unk8E0;             // A0 (8E0)
		void*         unk8E8;             // A8 (8E8)
		void*         unk8F0;             // B0 (8F0)
		float         unk8F8;             // B8 (8F8)  1.0 in power armor, and it flips on the very frame the transition happens
		float         unk8FC;             // BC (8FC)
		float         unk900;             // C0 (900)
		float         unk904;             // C4 (904)
		// The cooldown the VR hands run after a melee swing or a throw. It is named because the two
		// functions that arm it load the value straight out of a game setting: 0xF16A70 (the player
		// update, right where it decides whether the Pip-Boy is projected) writes fMeleeCooldown:VRInput
		// into 908, and 0xEFA3F8 writes fThrowCooldown:VRInput into both 908 and 90C and sets 914.
		// Both default to 1.0, which is exactly the one-second 1 -> 0 ramp seen in game.
		float         actionCooldown;             // C8 (908)  seconds left before the hands may melee or throw again, counted down to 0
		float         actionCooldownDuration;     // CC (90C)  what it was armed with, so a 0..1 fraction can be taken off it
		float         unk910;                     // D0 (910)  ctor seeds FLT_MAX
		std::uint8_t  unk914;                     // D4 (914)  set to 1 by the throw arming, alongside the two cooldown fields, and cleared by the ctor
		std::byte     pad915[0x3];                // D5 (915)
		void*         unk918;                     // D8 (918)
		std::uint32_t unk920;                     // E0 (920)
		std::uint32_t unk924;                     // E4 (924)
		std::uint32_t unk928;                     // E8 (928)
		std::uint32_t unk92C;                     // EC (92C)
		void*         unk930;                     // F0 (930)  held a pointer into the module in game
		void*         unk938;                     // F8 (938)
		void*         unk940;                     // 100 (940)
		std::uint8_t  unk948;                     // 108 (948)
		std::byte     pad949[0x3];                // 109 (949)
		std::int32_t  unk94C;                     // 10C (94C)  ctor seeds -1
		std::uint16_t unk950;                     // 110 (950)
		std::byte     pad952[0x2];                // 112 (952)
		std::int32_t  unk954;                     // 114 (954)  ctor seeds -1
		std::uint16_t unk958;                     // 118 (958)
		std::byte     pad95A[0x6];                // 11A (95A)
		NiNode*       fistHelperMaleLeft;         // 120 (960)  "fist_M_Left_HELPER"
		NiNode*       fistHelperMaleRight;        // 128 (968)  "fist_M_Right_HELPER"
		NiNode*       fistHelperFemaleLeft;       // 130 (970)  "fist_F_Left_HELPER"
		NiNode*       fistHelperFemaleRight;      // 138 (978)  "fist_F_Right_HELPER"
		NiNode*       fistHelperPowerArmorLeft;   // 140 (980)  "PA_fist_L_HELPER"
		NiNode*       fistHelperPowerArmorRight;  // 148 (988)  "PA_fist_R_HELPER"
		std::uint8_t  unk990;                     // 150 (990)
		std::byte     pad991[0x7];                // 151 (991)
		void*         unk998;                     // 158 (998)
		NiPoint3A     unk9A0;                     // 160 (9A0)
		std::uint32_t unk9B0;                     // 170 (9B0)
		std::uint32_t unk9B4;                     // 174 (9B4)
		void*         unk9B8;                     // 178 (9B8)
		NiPoint3A     unk9C0;                     // 180 (9C0)
	};
	// The size is what pins this struct: it puts every later member of PlayerCharacter where the
	// asserts below the class expect them. The per-field asserts are kept only for the named ones,
	// since asserting an offset that the declarations above already determine proves nothing.
	static_assert(sizeof(VRPlayerState) == 0x190);
	static_assert(offsetof(VRPlayerState, uiAnchorYaw) == 0x50);
	static_assert(offsetof(VRPlayerState, uiHeadYaw) == 0x54);
	static_assert(offsetof(VRPlayerState, snapTurnTargetYaw) == 0x88);
	static_assert(offsetof(VRPlayerState, rightHandedMode) == 0x8C);
	static_assert(offsetof(VRPlayerState, leftHandedMode) == 0x90);
	static_assert(offsetof(VRPlayerState, fistHelperMaleLeft) == 0x120);
	static_assert(offsetof(VRPlayerState, fistHelperMaleRight) == 0x128);
	static_assert(offsetof(VRPlayerState, fistHelperFemaleLeft) == 0x130);
	static_assert(offsetof(VRPlayerState, fistHelperFemaleRight) == 0x138);
	static_assert(offsetof(VRPlayerState, fistHelperPowerArmorLeft) == 0x140);
	static_assert(offsetof(VRPlayerState, fistHelperPowerArmorRight) == 0x148);
#endif
	// f4sevr-port: the base offsets below are the flat game's. VR has one more 8-byte polymorphic base
	// (8 of them, 490-4C8) and six BSTEventSource bases instead of four (4D0-6E0, 0x58 each), so its own
	// members start at 6E0, not 628 - the first 0xB8 of the +0x470 those members carry.
	class __declspec(novtable) PlayerCharacter :
		public Actor,                                                 // 000
		public BSTEventSource<BGSActorCellEvent>,                     // 4C8
		public BSTEventSource<BGSActorDeathEvent>,                    // 520
		public BSTEventSource<PositionPlayerEvent>,                   // 578
		public BSTEventSource<PickRefUpdateEvent>,                    // 5D0
		public BSTEventSink<MenuOpenCloseEvent>,                      // 490
		public BSTEventSink<MenuModeChangeEvent>,                     // 498
		public BSTEventSink<UserEventEnabledEvent>,                   // 4A0
		public BSTEventSink<OtherEventEnabledEvent>,                  // 4A8
		public BSTEventSink<TESHitEvent>,                             // 4B0
		public BSTEventSink<PerkValueEvents::PerkEntryUpdatedEvent>,  // 4B8
		public IMovementPlayerControlsFilter                          // 4C0
	{
	public:
		static constexpr auto RTTI{ RTTI::PlayerCharacter };
		static constexpr auto VTABLE{ VTABLE::PlayerCharacter };
		static constexpr auto FORM_ID{ ENUM_FORM_ID::kACHR };

		enum class GrabbingType;

		struct PlayerActionObject
		{
		public:
			// members
			float                                     timer;   // 0
			ObjectRefHandle                           refObj;  // 4
			REX::EnumSet<PLAYER_ACTION, std::int32_t> next;    // 8
		};
		static_assert(sizeof(PlayerActionObject) == 0xC);

		struct ScopedInventoryChangeMessageContext
		{
		public:
			ScopedInventoryChangeMessageContext(bool a_suppressMessages, bool a_suppressAudio) :
				suppressMessages(a_suppressMessages), suppressAudio(a_suppressAudio)
			{
				ctor(suppressMessages, suppressAudio);
			}

			~ScopedInventoryChangeMessageContext()
			{
				dtor();
			}

			// members
			const bool suppressMessages{ false };  // 00
			const bool suppressAudio{ false };     // 01

		private:
			void ctor(bool a_suppressMessages, bool a_suppressAudio)
			{
				using func_t = decltype(&ScopedInventoryChangeMessageContext::ctor);
				static REL::Relocation<func_t> func{ REL::ID(1512872) };
				return func(this, a_suppressMessages, a_suppressAudio);
			}

			void dtor()
			{
				using func_t = decltype(&ScopedInventoryChangeMessageContext::dtor);
				static REL::Relocation<func_t> func{ REL::ID(542100) };
				return func(this);
			}
		};
		static_assert(sizeof(ScopedInventoryChangeMessageContext) == 0x2);

		// add
		virtual void          InitiateSpectator(Actor*) { return; }                      // 133
		virtual std::uint32_t GetViolentCrimeGoldValue(const TESFaction* a_faction);     // 134
		virtual std::uint32_t GetNonViolentCrimeGoldValue(const TESFaction* a_faction);  // 135
		virtual void          ClearAllCrimeGold(TESFaction* a_faction);                  // 136

		[[nodiscard]] static PlayerCharacter* GetSingleton()
		{
			static REL::Relocation<NiPointer<PlayerCharacter>*> singleton{ REL::RelocationID(303410, 2690919) };
			return singleton->get();
		}

		[[nodiscard]] static ActorHandle GetPlayerHandle()
		{
			static REL::Relocation<ActorHandle*> singleton{ REL::RelocationID(522947, 2698072) };
			return *singleton;
		}

		void ClearPrison()
		{
			using func_t = decltype(&PlayerCharacter::ClearPrison);
			static REL::Relocation<func_t> func{ REL::RelocationID(0, 2233196) };
			return func(this);
		}

		DIFFICULTY_LEVEL GetDifficultyLevel()
		{
			using func_t = decltype(&PlayerCharacter::GetDifficultyLevel);
			static REL::Relocation<func_t> func{ REL::RelocationID(922962, 2233056) };
			return func(this);
		}

		bool IsGodMode()
		{
			using func_t = decltype(&PlayerCharacter::IsGodMode);
			static REL::Relocation<func_t> func{ REL::RelocationID(1032309, 2232986) };
			return func(this);
		}

		bool IsHolotapePlaying(BGSNote* a_holotape)
		{
			using func_t = decltype(&PlayerCharacter::IsHolotapePlaying);
			static REL::Relocation<func_t> func{ REL::RelocationID(530826, 2233206) };
			return func(this, a_holotape);
		}

		bool IsImmortal()
		{
			using func_t = decltype(&PlayerCharacter::IsImmortal);
			static REL::Relocation<func_t> func{ REL::RelocationID(500346, 2232988) };
			return func(this);
		}

		bool IsLockedOutOfTerminal(ObjectRefHandle a_handle);

		bool IsPipboyLightOn()
		{
			using func_t = decltype(&PlayerCharacter::IsPipboyLightOn);
			static REL::Relocation<func_t> func{ REL::ID(426550) };
			return func(this);
		}

		void LockOutOfTerminal(ObjectRefHandle a_handle);

		void PauseHolotape(BGSNote* a_holotape)
		{
			using func_t = decltype(&PlayerCharacter::PauseHolotape);
			static REL::Relocation<func_t> func{ REL::RelocationID(1567456, 2233208) };
			return func(this, a_holotape);
		}

		void PlayHolotape(BGSNote* a_holotape)
		{
			using func_t = decltype(&PlayerCharacter::PlayHolotape);
			static REL::Relocation<func_t> func{ REL::RelocationID(1581042, 2233207) };
			return func(this, a_holotape);
		}

		void QueueFastTravel(ObjectRefHandle a_marker, bool a_allowAutoSave)
		{
			using func_t = decltype(&PlayerCharacter::QueueFastTravel);
			static REL::Relocation<func_t> func{ REL::ID(556824) };
			return func(this, a_marker, a_allowAutoSave);
		}

		void RemoveLastUsedPowerArmor()
		{
			using func_t = decltype(&PlayerCharacter::RemoveLastUsedPowerArmor);
			static REL::Relocation<func_t> func{ REL::ID(1488486) };
			return func(this);
		}

		void SelectPerk(TESFormID a_formID, std::int8_t a_rank = 0)
		{
			using func_t = decltype(&PlayerCharacter::SelectPerk);
			static REL::Relocation<func_t> func{ REL::ID(1397326) };
			return func(this, a_formID, a_rank);
		}

		void SetEscaping(bool a_flag, bool a_escaped)
		{
			using func_t = decltype(&PlayerCharacter::SetEscaping);
			static REL::Relocation<func_t> func{ REL::ID(25528) };
			return func(this, a_flag, a_escaped);
		}

		void SetLastDialogueInput(uint32_t dialogueOption)
		{
			using func_t = decltype(&PlayerCharacter::SetLastDialogueInput);
			static REL::Relocation<func_t> func{ REL::RelocationID(696117, 2233190) };
			return func(this, dialogueOption);
		}

		void SetVATSCriticalCount(uint32_t a_critCount)
		{
			using func_t = decltype(&PlayerCharacter::SetVATSCriticalCount);
			static REL::Relocation<func_t> func{ REL::ID(327338) };
			return func(this, a_critCount);
		}

		void ShowPipboyLight(bool a_show, bool a_skipEffects)
		{
			using func_t = decltype(&PlayerCharacter::ShowPipboyLight);
			static REL::Relocation<func_t> func{ REL::ID(1304102) };
			return func(this, a_show, a_skipEffects);
		}

		void TogglePipBoyLight(bool a_unk)
		{
			using func_t = decltype(&PlayerCharacter::TogglePipBoyLight);
			static REL::Relocation<func_t> func{ REL::ID(520007) };
			return func(this, a_unk);
		}

		// members
#ifdef ENABLE_FALLOUT_VR
		// f4sevr-port: VR layout, read out of Fallout4VR.exe 1.2.72's PlayerCharacter ctor (0xEED4F0),
		// which walks the members in declaration order. Two blocks of VR-only state bracket the two
		// locks below; with the 0xB8 of extra base subobjects they make up the shift the rest carries.
		// The 0xB8 comes first, because this header declares the flat game's bases: the compiler ends them
		// at 628, while VR's own bases run to 6E0. Only then does the node table start.
		//
		// What each block holds is documented on VRPlayerState and on vrPlayerStateBack below.
		std::byte     vrExtraBases[0xB8];  // 628 (VR only), the bases this header cannot express
		VRPlayerNodes vrNodes;             // VR 6E0
		VRPlayerState vrState;             // VR 840, through VR 9D0
#endif
		BSSpinLock actorToDisplayOnHUDLock;  // flat: 628  vr: 9D0
		BSSpinLock questTargetLock;          // flat: 630  vr: 9D8
#ifdef ENABLE_FALLOUT_VR
		// f4sevr-port: the second VR-only block takes the shift to 0x478. It drops back to 0x470 at
		// lastKnownGoodPosition: the 8 bytes flat pads for its alignment (6F8 -> 700) are already there
		// in VR, where factionOwnerFriendsMap ends on B70.
		std::byte vrPlayerStateMid[0xD0];  // 9E0 (VR only), through VR AB0
#endif
		BSTHashMap<const TESFaction*, CrimeGoldStruct>                crimeGoldMap;                         // flat: 638  vr: AB0
		BSTHashMap<const TESFaction*, StolenItemValueStruct>          stolenItemValueMap;                   // 668
		PlayerCoverData                                               coverData;                            // 698
		ObjectRefHandle                                               commandWaitMarker;                    // 6C4
		BSTHashMap<const TESFaction*, FriendshipFactionsStruct>       factionOwnerFriendsMap;               // 6C8
		NiPoint3A                                                     lastKnownGoodPosition;                // flat: 700  vr: B70
		NiPoint3A                                                     lastKnownGoodNavmeshPosition;         // 710
		NiPoint3                                                      bulletAutoAim;                        // 720
		NiPoint3                                                      cachedVelocity;                       // 72C
		BSTArray<ObjectRefHandle>                                     currentMapMarkers;                    // 738
		BSTArray<BSTTuple<NiPoint3, AITimeStamp>>                     velocityArray;                        // 750
		BSTArray<ProjectileHandle>                                    runesCast;                            // 768
		BSTArray<TESImageSpaceModifier*>                              animationImageSpaceModifiers;         // 78-
		BSTArray<ImageSpaceModifierInstanceForm*>                     animationImageSpaceInstances;         // 798
		BSTArray<BSTTuple<NiPointer<NiLight>, bool>>                  playerLights;                         // 7B0
		BSSimpleList<TESQuestStageItem*>                              questLog;                             // 7C8
		BSTArray<BGSInstancedQuestObjective>                          objectives;                           // 7D8
		BSTHashMap<TESQuest*, QuestTargetArray*>                      questTargets;                         // 7F0
		BSTHashMap<std::uint32_t, SayOnceTimeStampStruct>             currentSayOnceInfosMap;               // 820
		BSTHashMap<std::uint32_t, SayOnceTimeStampStruct>             currentSpeechRepeatMap;               // 850
		BSSimpleList<ObjectRefHandle>                                 droppedRefList;                       // 880
		NiTMap<std::uint32_t, std::uint8_t>                           randomDoorSpaceMap;                   // 890
		TESWorldSpace*                                                cachedWorldspace;                     // 8B0
		NiPoint3                                                      exteriorPosition;                     // 8B8
		BSTSmallArray<SubgraphHandle, 2>                              pipboyAnimSubGraph;                   // 8C8
		PLAYER_TARGET_LOC                                             queuedTargetLoc;                      // 8E8
		PLAYER_TARGET_LOC                                             queuedFlightLoc;                      // 948
		NiPoint3A                                                     flightGridCenter;                     // 9B0
		BSPathingSolution*                                            flightMountPathingSolution;           // 9C0
		const TESObjectCELL*                                          flightMountPreloadCell;               // 9C8
		BSSoundHandle                                                 magicFailureSound;                    // 9D0
		BSTArray<std::uint32_t>                                       rumorTopicInfoList;                   // 9D8
		std::uint32_t                                                 testChallangeInfoID;                  // 9F0
		DialoguePackage*                                              closestConversation;                  // 9F8
		DialoguePackage*                                              aiConversationRunning;                // A00
		std::int32_t                                                  numberofStealWarnings;                // A08
		float                                                         stealWarningTimer;                    // A0C
		std::int32_t                                                  numberofPickpocketWarnings;           // A10
		float                                                         pickPocketWarningTimer;               // A14
		AITimeStamp                                                   warnToLeaveTimeStamp;                 // A18
		AITimeStamp                                                   usingTeleportDoorTimeStamp;           // A1C
		NiPointer<ImageSpaceModifierInstanceDOF>                      ironsightsDOFInstance;                // A20
		TESFaction*                                                   currentPrisonFaction;                 // A28
		std::uint32_t                                                 jailSentence;                         // A30
		NiPointer<NiAVObject>                                         autoAimDebugSphere;                   // A38
		BGSNote*                                                      currHolotape;                         // A40
		BSSoundHandle                                                 soundHand;                            // A48
		std::int32_t                                                  vampireFeedDetection;                 // A50
		std::uint32_t                                                 mapMarkerIterator;                    // A54
		ObjectRefHandle                                               forceActivateRef;                     // A58
		ObjectRefHandle                                               loopingActivateRef;                   // A5C
		std::uint32_t                                                 dialogueCameraLastSceneID;            // A60
		PlayerActionObject                                            playerActionObjects[15];              // A64
		REX::EnumSet<PLAYER_ACTION, std::int32_t>                     mostRecentAction;                     // B18
		ActorHandle                                                   actorDoingPlayerCommand;              // B1C
		BSTValueEventSource<PlayerCommandTypeEvent>                   playerCurrentCommandType;             // B20
		BSTArray<hkRefPtr<hknpBSMouseSpringAction>>                   grabSprings;                          // B38
		NiPoint3                                                      grabUserRotation;                     // B50
		ObjectRefHandle                                               grabbedObject;                        // B5C
		float                                                         grabObjectWeight;                     // B60
		float                                                         grabDistance;                         // B64
		std::uint32_t                                                 secondsToSleepPerUpdate;              // B68
		std::uint32_t                                                 sleepSeconds;                         // B6C
		BSTSmartPointer<BipedAnim>                                    firstPersonBipedAnim;                 // flat: B70  vr: FE0
		NiPointer<NiNode>                                             firstPerson3D;                        // B78
		NiAVObject*                                                   firstPersonTorso;                     // B80
		NiAVObject*                                                   firstPersonEye;                       // B88
		float                                                         eyeHeight;                            // B90
		float                                                         greetTimer;                           // B94
		float                                                         encumberedTimer;                      // B98
		float                                                         powerAttackTimer;                     // B9C
		float                                                         autoReloadTimer;                      // BA0
		float                                                         pivotToFaceCamera;                    // BA4
		std::int32_t                                                  hourstosleep;                         // BA8
		std::int32_t                                                  amountStolenSold;                     // BAC
		std::uint32_t                                                 valueStolen;                          // BB0
		ActorHandle                                                   lastRiddenMount;                      // BB4
		ActorHandle                                                   lightTarget;                          // BB8
		float                                                         sortActorDistanceTimer;               // BBC
		float                                                         sitHeadingDelta;                      // BC0
		ObjectRefHandle                                               playerMapMarker;                      // BC4
		TeleportPath*                                                 playerMarkerPath;                     // BC8
		std::uint32_t                                                 skillTrainingsThisLevel;              // BD0
		TESClass*                                                     defaultClass;                         // BD8
		std::uint32_t                                                 crimeCounts[7];                       // BE0
		AlchemyItem*                                                  pendingPoison;                        // C00
		std::time_t                                                   lastPlayingTimeUpdate;                // C08
		std::time_t                                                   totalPlayingTime;                     // C10
		std::uint32_t                                                 characterSeed;                        // C18
		TESForm*                                                      lastKnownGoodLocation;                // C20
		NiPointer<BSLight>                                            firstPersonLight;                     // C28
		NiPointer<BSLight>                                            thirdPersonLight;                     // C30
		NiPointer<BSLight>                                            pipboyLight;                          // C38
		NiPointer<NiLight>                                            niPipboyLight;                        // C40
		float                                                         dropAngleMod;                         // C48
		float                                                         lastDropAngleMod;                     // C4C
		BSTValueEventSource<PlayerAutoAimActorEvent>                  autoAimActor;                         // C50
		ObjectRefHandle                                               homingTarget;                         // C68
		float                                                         homingTargetTimer;                    // C6C
		NiPointer<NiAVObject>                                         targeted3D;                           // C70
		CombatGroup*                                                  combatGroup;                          // C78
		BSTArray<ActorHandle>                                         actorsToDisplayOnTheHUD;              // C80
		BSTArray<EquippedItem>                                        lastOneHandItems;                     // C98
		std::uint32_t                                                 teammateCount;                        // CB0
		float                                                         combatTimer;                          // CB4
		float                                                         yieldTimer;                           // CB8
		float                                                         chaseTimer;                           // CBC
		float                                                         drawSheatheSafetyTimer;               // CC0
		std::uint32_t                                                 activeCombatantCount;                 // CC4
		BGSLocation*                                                  currentLocation;                      // CC8
		AITimeStamp                                                   cachedVelocityTimeStamp;              // CD0
		float                                                         telekinesisDistance;                  // CD4
		float                                                         commandTimer;                         // CD8
		float                                                         dialogueCameraChangeRate;             // CDC
		float                                                         fireWeaponEventTimer;                 // CE0
		REX::EnumSet<DEFAULT_OBJECT, std::int32_t>                    animationObjectAction;                // CE4
		REX::EnumSet<GrabbingType, std::int32_t>                      grabType;                             // CE8
		ActorHandle                                                   assumedIdentity;                      // CEC
		std::int8_t                                                   murder;                               // CF0
		std::int8_t                                                   perkCount;                            // CF1
		std::int8_t                                                   byCharGenFlag;                        // CF2
		Crime*                                                        resistArrestCrime;                    // CF8
		BGSCharacterTint::Entries*                                    tintingData;                          // D00
		BGSTextureSet*                                                complexion;                           // D08
		TESRace*                                                      charGenRace;                          // D10
		EffectSetting*                                                currentAlchemyEffect;                 // D18
		ItemChange*                                                   currentTemperingInventoryItem;        // D20
		TESObjectREFR*                                                commandTarget;                        // D28
		TESObjectREFR*                                                walkThroughDoor;                      // D30
		REX::EnumSet<COMMAND_TYPE, std::int32_t>                      currentCommand;                       // D38
		REX::EnumSet<SCENE_ACTION_PLAYER_RESPONSE_TYPE, std::int32_t> playerDialogueInput;                  // D3C
		ActorValueInfo*                                               stimpakTarget;                        // D40
		float                                                         vatsCriticalCharge;                   // D48
		std::uint32_t                                                 vatsCriticalCount;                    // D4C
		std::uint32_t                                                 maxVATSCriticalCount;                 // D50
		float                                                         timeBlendingOutFirstPersonOffset;     // D54
		ActorHandle                                                   lastMagnetismActor;                   // D58
		NiPoint3                                                      magnetismActorPos;                    // D5C
		ObjectRefHandle                                               workstationBeingLeft;                 // D68
		ActorHandle                                                   actorLeavingWorkstation;              // D6C
		BGSScene*                                                     allowDialogueCameraScene;             // D70
		ActorHandle                                                   helloActor;                           // D78
		TESObjectREFR*                                                weaponAmmoRef;                        // D80
		std::int32_t                                                  equippingAmmoCount;                   // D88
		float                                                         dialogueCameraResumeTimer;            // D8C
		ObjectRefHandle                                               lastUsedPowerArmor;                   // D90
		TeleportPath*                                                 lastUsedPowerArmorMarkerPath;         // D98
		TESObjectWEAP*                                                lastUsedThrownWeapon;                 // DA0
		BSSoundHandle                                                 powerArmorInteriorSoundHandle;        // DA8
		BSTArray<BSTTuple<ObjectRefHandle, float>>                    lockedTerminals;                      // DB0
		BSTArray<BSTTuple<std::int32_t, std::int32_t>>                boneMapping1stTo3rd;                  // DC8
		BSTOptional<XPChangeData>                                     syncedXPChange;                       // DE0
		std::int32_t                                                  queuedSwitch: 1;                      // DF4:00
		std::uint32_t                                                 charGenHUDMode;                       // DF8
		bool                                                          insideMemoryHUDMode: 1;               // DFC:0
		bool                                                          xpSyncedWithHUDNotification: 1;       // DFC:1
		bool                                                          travelUseDoor: 1;                     // DFC:2
		bool                                                          transporting: 1;                      // DFC:3
		bool                                                          overAutoAimTarget: 1;                 // DFC:4
		bool                                                          showQuestItemsInInventory: 1;         // DFC:5
		bool                                                          hostileDetection: 1;                  // DFC:6
		bool                                                          escaping: 1;                          // DFC:7
		bool                                                          forceQuestTargetRepath: 1;            // DFD:0
		bool                                                          speaking: 1;                          // DFD:1
		bool                                                          isSleeping: 1;                        // DFD:2
		bool                                                          actorinSneakRange: 1;                 // DFD:3
		bool                                                          greetingPlayer: 1;                    // DFD:4
		bool                                                          beenAttacked: 1;                      // DFD:5
		bool                                                          allowEGMCacheClear: 1;                // DFD:6
		bool                                                          aiControlledToPos: 1;                 // DFD:7
		bool                                                          aiControlledFromPos: 1;               // DFE:0
		bool                                                          aiControlledPackage: 1;               // DFE:1
		bool                                                          returnToLastKnownGoodPosition: 1;     // DFE:2
		bool                                                          beingChased: 1;                       // DFE:3
		bool                                                          is3rdPersonModelShown: 1;             // DFE:4
		bool                                                          hideFirstPersonGeometry: 1;           // DFE:5
		bool                                                          insufficientChargeMessageShownL: 1;   // DFE:6
		bool                                                          insufficientChargeMessageShownR: 1;   // DFE:7
		bool                                                          target3DDistant: 1;                   // DFF:0
		bool                                                          playerInCombat: 1;                    // DFF:1
		bool                                                          attemptedYieldInCurrentCombat: 1;     // DFF:2
		bool                                                          currentAlchemyIsPoison: 1;            // DFF:3
		bool                                                          positioning: 1;                       // DFF:4
		bool                                                          crimeReporting: 1;                    // DFF:5
		bool                                                          healthTutorialShown: 1;               // DFF:6
		bool                                                          magickaTutorialShown: 1;              // DFF:7
		bool                                                          staminaTutorialShown: 1;              // E00:0
		bool                                                          goToJailQueued: 1;                    // E00:1
		bool                                                          doorAutosaveQueued: 1;                // E00:2
		bool                                                          sprintToggled: 1;                     // E00:3
		bool                                                          useHighSpeedLoading: 1;               // E00:4
		bool                                                          sceneActionActive: 1;                 // E00:5
		bool                                                          sayingHello: 1;                       // E00:6
		bool                                                          disabledRadio: 1;                     // E00:7
		bool                                                          updateAutoAimActor: 1;                // E01:0
		bool                                                          autoReload: 1;                        // E01:1
		bool                                                          usingScopeWithOverlay: 1;             // E01:2
		bool                                                          steadyingWeapon: 1;                   // E01:3
		bool                                                          speechChallengeReward: 1;             // E01:4
		bool                                                          blendOutFirstPersonOffset: 1;         // E01:5
		bool                                                          dialogueCameraConsiderResume: 1;      // E01:6
		bool                                                          shouldFollowerSupressIdleChatter: 1;  // E01:7
		bool                                                          idleChatter: 1;                       // E02:0
		bool                                                          switchingPerspectives: 1;             // E02:1
		bool                                                          onElevator: 1;                        // E02:2
		bool                                                          sayCommandResponse: 1;                // E02:3
		bool                                                          inLooksMenu: 1;                       // E02:4
		bool                                                          playingTimeFrozen: 1;                 // E02:5
		bool                                                          everModded: 1;                        // E02:6
#ifdef ENABLE_FALLOUT_VR
		// f4sevr-port: VR keeps 0x30 bytes of its own state past the last flat member, largely flags.
		// 12A0 / 12A4 is one 64-bit flag word; logging every bit that moved, frame by frame, separates
		// the state bits from the noise:
		//   12A0 & 0x02000000  flips every single frame, so parity or a double-buffer index, not state
		//   12A0 & 0x40000000  clear while a weapon is drawn
		//   12A0 & 0x80000000  set for the first half second after a full-screen menu closes
		//   12A4 & 0x00000001  1 while the player has control, 0 while a menu owns the view (lockpick,
		//                      crafting, Pip-Boy)
		//   12A4 & 0x00000040  the snap-turn request F4VROffsets.h writes, seen set for exactly one
		//                      frame at a time - the companion 0x20 the applier consumes never showed,
		//                      so it does not outlive the frame that sets it
		// Three more bits have a writer rather than an observation behind them: the player update at
		// 0xF16A70 sets 129D bit 0 and 12A5 bit 1, and tests 129E bit 4, around the same Pip-Boy and
		// melee work that arms VRPlayerState::actionCooldown.
		alignas(0x10) std::byte vrPlayerStateBack[0x30];  // 1280 (VR only)
#endif
	};
#ifdef ENABLE_FALLOUT_VR
	// f4sevr-port: VR layout, read from Fallout4VR.exe 1.2.72. The singleton is allocated 0x12B0 bytes
	// (at 0x140D8A308), not the 0x1280 a flat 0xE10 plus the 0x470 shift would give.
	static_assert(sizeof(PlayerCharacter) == 0x12B0);
	// The locks bracket the second VR block: 0x140F792E0 takes 9D0 around actorsToDisplayOnTheHUD at
	// 10F0, 0x1405101B0 takes 9D8 around questTargets at C60.
	// vrNodes is the one member whose own offset the size asserts cannot vouch for: it sits inside the
	// VR-only run, so misplacing it leaves every other offset and sizeof intact.
	static_assert(offsetof(PlayerCharacter, vrNodes) == 0x6E0);

	// Every member VR offset, generated from the flat offsets above plus the shift this file
	// documents, then checked against what the constructor at 0xEED4F0 actually writes. They are
	// spelled out rather than summarised because a single wrong type size would slide everything
	// after it while sizeof still matched, which is how this layout hid a bad offset before.
	static_assert(offsetof(PlayerCharacter, actorToDisplayOnHUDLock) == 0x9D0);
	static_assert(offsetof(PlayerCharacter, questTargetLock) == 0x9D8);
	static_assert(offsetof(PlayerCharacter, crimeGoldMap) == 0xAB0);
	static_assert(offsetof(PlayerCharacter, stolenItemValueMap) == 0xAE0);
	static_assert(offsetof(PlayerCharacter, coverData) == 0xB10);
	static_assert(offsetof(PlayerCharacter, commandWaitMarker) == 0xB3C);
	static_assert(offsetof(PlayerCharacter, factionOwnerFriendsMap) == 0xB40);
	static_assert(offsetof(PlayerCharacter, lastKnownGoodPosition) == 0xB70);
	static_assert(offsetof(PlayerCharacter, lastKnownGoodNavmeshPosition) == 0xB80);
	static_assert(offsetof(PlayerCharacter, bulletAutoAim) == 0xB90);
	static_assert(offsetof(PlayerCharacter, cachedVelocity) == 0xB9C);
	static_assert(offsetof(PlayerCharacter, currentMapMarkers) == 0xBA8);
	static_assert(offsetof(PlayerCharacter, velocityArray) == 0xBC0);
	static_assert(offsetof(PlayerCharacter, runesCast) == 0xBD8);
	static_assert(offsetof(PlayerCharacter, animationImageSpaceInstances) == 0xC08);
	static_assert(offsetof(PlayerCharacter, playerLights) == 0xC20);
	static_assert(offsetof(PlayerCharacter, questLog) == 0xC38);
	static_assert(offsetof(PlayerCharacter, objectives) == 0xC48);
	static_assert(offsetof(PlayerCharacter, questTargets) == 0xC60);
	static_assert(offsetof(PlayerCharacter, currentSayOnceInfosMap) == 0xC90);
	static_assert(offsetof(PlayerCharacter, currentSpeechRepeatMap) == 0xCC0);
	static_assert(offsetof(PlayerCharacter, droppedRefList) == 0xCF0);
	static_assert(offsetof(PlayerCharacter, randomDoorSpaceMap) == 0xD00);
	static_assert(offsetof(PlayerCharacter, cachedWorldspace) == 0xD20);
	static_assert(offsetof(PlayerCharacter, exteriorPosition) == 0xD28);
	static_assert(offsetof(PlayerCharacter, pipboyAnimSubGraph) == 0xD38);
	static_assert(offsetof(PlayerCharacter, queuedTargetLoc) == 0xD58);
	static_assert(offsetof(PlayerCharacter, queuedFlightLoc) == 0xDB8);
	static_assert(offsetof(PlayerCharacter, flightGridCenter) == 0xE20);
	static_assert(offsetof(PlayerCharacter, flightMountPathingSolution) == 0xE30);
	static_assert(offsetof(PlayerCharacter, flightMountPreloadCell) == 0xE38);
	static_assert(offsetof(PlayerCharacter, magicFailureSound) == 0xE40);
	static_assert(offsetof(PlayerCharacter, rumorTopicInfoList) == 0xE48);
	static_assert(offsetof(PlayerCharacter, testChallangeInfoID) == 0xE60);
	static_assert(offsetof(PlayerCharacter, closestConversation) == 0xE68);
	static_assert(offsetof(PlayerCharacter, aiConversationRunning) == 0xE70);
	static_assert(offsetof(PlayerCharacter, numberofStealWarnings) == 0xE78);
	static_assert(offsetof(PlayerCharacter, stealWarningTimer) == 0xE7C);
	static_assert(offsetof(PlayerCharacter, numberofPickpocketWarnings) == 0xE80);
	static_assert(offsetof(PlayerCharacter, pickPocketWarningTimer) == 0xE84);
	static_assert(offsetof(PlayerCharacter, warnToLeaveTimeStamp) == 0xE88);
	static_assert(offsetof(PlayerCharacter, usingTeleportDoorTimeStamp) == 0xE8C);
	static_assert(offsetof(PlayerCharacter, ironsightsDOFInstance) == 0xE90);
	static_assert(offsetof(PlayerCharacter, currentPrisonFaction) == 0xE98);
	static_assert(offsetof(PlayerCharacter, jailSentence) == 0xEA0);
	static_assert(offsetof(PlayerCharacter, autoAimDebugSphere) == 0xEA8);
	static_assert(offsetof(PlayerCharacter, currHolotape) == 0xEB0);
	static_assert(offsetof(PlayerCharacter, soundHand) == 0xEB8);
	static_assert(offsetof(PlayerCharacter, vampireFeedDetection) == 0xEC0);
	static_assert(offsetof(PlayerCharacter, mapMarkerIterator) == 0xEC4);
	static_assert(offsetof(PlayerCharacter, forceActivateRef) == 0xEC8);
	static_assert(offsetof(PlayerCharacter, loopingActivateRef) == 0xECC);
	static_assert(offsetof(PlayerCharacter, dialogueCameraLastSceneID) == 0xED0);
	static_assert(offsetof(PlayerCharacter, playerActionObjects) == 0xED4);
	static_assert(offsetof(PlayerCharacter, mostRecentAction) == 0xF88);
	static_assert(offsetof(PlayerCharacter, actorDoingPlayerCommand) == 0xF8C);
	static_assert(offsetof(PlayerCharacter, playerCurrentCommandType) == 0xF90);
	static_assert(offsetof(PlayerCharacter, grabSprings) == 0xFA8);
	static_assert(offsetof(PlayerCharacter, grabUserRotation) == 0xFC0);
	static_assert(offsetof(PlayerCharacter, grabbedObject) == 0xFCC);
	static_assert(offsetof(PlayerCharacter, grabObjectWeight) == 0xFD0);
	static_assert(offsetof(PlayerCharacter, grabDistance) == 0xFD4);
	static_assert(offsetof(PlayerCharacter, secondsToSleepPerUpdate) == 0xFD8);
	static_assert(offsetof(PlayerCharacter, sleepSeconds) == 0xFDC);
	static_assert(offsetof(PlayerCharacter, firstPersonBipedAnim) == 0xFE0);
	static_assert(offsetof(PlayerCharacter, firstPerson3D) == 0xFE8);
	static_assert(offsetof(PlayerCharacter, firstPersonTorso) == 0xFF0);
	static_assert(offsetof(PlayerCharacter, firstPersonEye) == 0xFF8);
	static_assert(offsetof(PlayerCharacter, eyeHeight) == 0x1000);
	static_assert(offsetof(PlayerCharacter, greetTimer) == 0x1004);
	static_assert(offsetof(PlayerCharacter, encumberedTimer) == 0x1008);
	static_assert(offsetof(PlayerCharacter, powerAttackTimer) == 0x100C);
	static_assert(offsetof(PlayerCharacter, autoReloadTimer) == 0x1010);
	static_assert(offsetof(PlayerCharacter, pivotToFaceCamera) == 0x1014);
	static_assert(offsetof(PlayerCharacter, hourstosleep) == 0x1018);
	static_assert(offsetof(PlayerCharacter, amountStolenSold) == 0x101C);
	static_assert(offsetof(PlayerCharacter, valueStolen) == 0x1020);
	static_assert(offsetof(PlayerCharacter, lastRiddenMount) == 0x1024);
	static_assert(offsetof(PlayerCharacter, lightTarget) == 0x1028);
	static_assert(offsetof(PlayerCharacter, sortActorDistanceTimer) == 0x102C);
	static_assert(offsetof(PlayerCharacter, sitHeadingDelta) == 0x1030);
	static_assert(offsetof(PlayerCharacter, playerMapMarker) == 0x1034);
	static_assert(offsetof(PlayerCharacter, playerMarkerPath) == 0x1038);
	static_assert(offsetof(PlayerCharacter, skillTrainingsThisLevel) == 0x1040);
	static_assert(offsetof(PlayerCharacter, defaultClass) == 0x1048);
	static_assert(offsetof(PlayerCharacter, crimeCounts) == 0x1050);
	static_assert(offsetof(PlayerCharacter, pendingPoison) == 0x1070);
	static_assert(offsetof(PlayerCharacter, lastPlayingTimeUpdate) == 0x1078);
	static_assert(offsetof(PlayerCharacter, totalPlayingTime) == 0x1080);
	static_assert(offsetof(PlayerCharacter, characterSeed) == 0x1088);
	static_assert(offsetof(PlayerCharacter, lastKnownGoodLocation) == 0x1090);
	static_assert(offsetof(PlayerCharacter, firstPersonLight) == 0x1098);
	static_assert(offsetof(PlayerCharacter, thirdPersonLight) == 0x10A0);
	static_assert(offsetof(PlayerCharacter, pipboyLight) == 0x10A8);
	static_assert(offsetof(PlayerCharacter, niPipboyLight) == 0x10B0);
	static_assert(offsetof(PlayerCharacter, dropAngleMod) == 0x10B8);
	static_assert(offsetof(PlayerCharacter, lastDropAngleMod) == 0x10BC);
	static_assert(offsetof(PlayerCharacter, autoAimActor) == 0x10C0);
	static_assert(offsetof(PlayerCharacter, homingTarget) == 0x10D8);
	static_assert(offsetof(PlayerCharacter, homingTargetTimer) == 0x10DC);
	static_assert(offsetof(PlayerCharacter, targeted3D) == 0x10E0);
	static_assert(offsetof(PlayerCharacter, combatGroup) == 0x10E8);
	static_assert(offsetof(PlayerCharacter, actorsToDisplayOnTheHUD) == 0x10F0);
	static_assert(offsetof(PlayerCharacter, lastOneHandItems) == 0x1108);
	static_assert(offsetof(PlayerCharacter, teammateCount) == 0x1120);
	static_assert(offsetof(PlayerCharacter, combatTimer) == 0x1124);
	static_assert(offsetof(PlayerCharacter, yieldTimer) == 0x1128);
	static_assert(offsetof(PlayerCharacter, chaseTimer) == 0x112C);
	static_assert(offsetof(PlayerCharacter, drawSheatheSafetyTimer) == 0x1130);
	static_assert(offsetof(PlayerCharacter, activeCombatantCount) == 0x1134);
	static_assert(offsetof(PlayerCharacter, currentLocation) == 0x1138);
	static_assert(offsetof(PlayerCharacter, cachedVelocityTimeStamp) == 0x1140);
	static_assert(offsetof(PlayerCharacter, telekinesisDistance) == 0x1144);
	static_assert(offsetof(PlayerCharacter, commandTimer) == 0x1148);
	static_assert(offsetof(PlayerCharacter, dialogueCameraChangeRate) == 0x114C);
	static_assert(offsetof(PlayerCharacter, fireWeaponEventTimer) == 0x1150);
	static_assert(offsetof(PlayerCharacter, animationObjectAction) == 0x1154);
	static_assert(offsetof(PlayerCharacter, grabType) == 0x1158);
	static_assert(offsetof(PlayerCharacter, assumedIdentity) == 0x115C);
	static_assert(offsetof(PlayerCharacter, murder) == 0x1160);
	static_assert(offsetof(PlayerCharacter, perkCount) == 0x1161);
	static_assert(offsetof(PlayerCharacter, byCharGenFlag) == 0x1162);
	static_assert(offsetof(PlayerCharacter, resistArrestCrime) == 0x1168);
	static_assert(offsetof(PlayerCharacter, tintingData) == 0x1170);
	static_assert(offsetof(PlayerCharacter, complexion) == 0x1178);
	static_assert(offsetof(PlayerCharacter, charGenRace) == 0x1180);
	static_assert(offsetof(PlayerCharacter, currentAlchemyEffect) == 0x1188);
	static_assert(offsetof(PlayerCharacter, currentTemperingInventoryItem) == 0x1190);
	static_assert(offsetof(PlayerCharacter, commandTarget) == 0x1198);
	static_assert(offsetof(PlayerCharacter, walkThroughDoor) == 0x11A0);
	static_assert(offsetof(PlayerCharacter, currentCommand) == 0x11A8);
	static_assert(offsetof(PlayerCharacter, playerDialogueInput) == 0x11AC);
	static_assert(offsetof(PlayerCharacter, stimpakTarget) == 0x11B0);
	static_assert(offsetof(PlayerCharacter, vatsCriticalCharge) == 0x11B8);
	static_assert(offsetof(PlayerCharacter, vatsCriticalCount) == 0x11BC);
	static_assert(offsetof(PlayerCharacter, maxVATSCriticalCount) == 0x11C0);
	static_assert(offsetof(PlayerCharacter, timeBlendingOutFirstPersonOffset) == 0x11C4);
	static_assert(offsetof(PlayerCharacter, lastMagnetismActor) == 0x11C8);
	static_assert(offsetof(PlayerCharacter, magnetismActorPos) == 0x11CC);
	static_assert(offsetof(PlayerCharacter, workstationBeingLeft) == 0x11D8);
	static_assert(offsetof(PlayerCharacter, actorLeavingWorkstation) == 0x11DC);
	static_assert(offsetof(PlayerCharacter, allowDialogueCameraScene) == 0x11E0);
	static_assert(offsetof(PlayerCharacter, helloActor) == 0x11E8);
	static_assert(offsetof(PlayerCharacter, weaponAmmoRef) == 0x11F0);
	static_assert(offsetof(PlayerCharacter, equippingAmmoCount) == 0x11F8);
	static_assert(offsetof(PlayerCharacter, dialogueCameraResumeTimer) == 0x11FC);
	static_assert(offsetof(PlayerCharacter, lastUsedPowerArmor) == 0x1200);
	static_assert(offsetof(PlayerCharacter, lastUsedPowerArmorMarkerPath) == 0x1208);
	static_assert(offsetof(PlayerCharacter, lastUsedThrownWeapon) == 0x1210);
	static_assert(offsetof(PlayerCharacter, powerArmorInteriorSoundHandle) == 0x1218);
	static_assert(offsetof(PlayerCharacter, lockedTerminals) == 0x1220);
	static_assert(offsetof(PlayerCharacter, boneMapping1stTo3rd) == 0x1238);
	static_assert(offsetof(PlayerCharacter, syncedXPChange) == 0x1250);
	static_assert(offsetof(PlayerCharacter, charGenHUDMode) == 0x1268);
	static_assert(offsetof(PlayerCharacter, actorToDisplayOnHUDLock) == 0x9D0);
	static_assert(offsetof(PlayerCharacter, questTargetLock) == 0x9D8);
	static_assert(offsetof(PlayerCharacter, crimeGoldMap) == 0xAB0);
	static_assert(offsetof(PlayerCharacter, lastKnownGoodPosition) == 0xB70);
	static_assert(offsetof(PlayerCharacter, questTargets) == 0xC60);
	static_assert(offsetof(PlayerCharacter, firstPersonBipedAnim) == 0xFE0);
	static_assert(offsetof(PlayerCharacter, firstPerson3D) == 0xFE8);
	// Read from Fallout4VR.exe: IsPipboyLightOn 0xF27790 tests pipboyLight, ShowPipboyLight 0xF277B0 writes both.
	static_assert(offsetof(PlayerCharacter, pipboyLight) == 0x10A8);
	static_assert(offsetof(PlayerCharacter, niPipboyLight) == 0x10B0);
	static_assert(offsetof(PlayerCharacter, actorsToDisplayOnTheHUD) == 0x10F0);
	static_assert(offsetof(PlayerCharacter, tintingData) == 0x1170);
#else
	static_assert(sizeof(PlayerCharacter) == 0xE10);
#endif
}
