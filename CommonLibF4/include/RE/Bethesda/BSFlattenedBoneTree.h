#pragma once

#include "RE/Bethesda/BSFixedString.h"
#include "RE/Bethesda/BSStringPool.h"
#include "RE/NetImmerse/NiNode.h"
#include "RE/NetImmerse/NiTransform.h"

namespace RE
{
	// The node the game puts at the top of an actor's skeleton (the first child of its 3D root): it keeps every
	// bone's local and world transform in one flat array, which is what skinning reads, so moving a bone's node
	// takes effect once UpdateBoneArray copies the nodes into the array.
	class __declspec(novtable) BSFlattenedBoneTree :
		public NiNode  // 000
	{
	public:
		static constexpr auto RTTI{ RTTI::BSFlattenedBoneTree };
		static constexpr auto VTABLE{ VTABLE::BSFlattenedBoneTree };
		static constexpr auto Ni_RTTI{ Ni_RTTI::BSFlattenedBoneTree };

		struct BoneTransforms
		{
		public:
			// members
			NiTransform   local;     // 00
			NiTransform   world;     // 40
			std::int16_t  parPos;    // 80 - the parent bone's index, -1 for the root
			std::int16_t  childPos;  // 82
			std::uint32_t unk84;     // 84
			NiNode*       refNode;   // 88 - the bone's node
			BSFixedString name;      // 90
			std::uint64_t unk98;     // 98
		};
		static_assert(sizeof(BoneTransforms) == 0xA0);

		struct BoneNodePosition
		{
		public:
			// members
			BSStringPool::Entry* name;      // 00
			std::int32_t         position;  // 08
			std::uint32_t        pad0C;     // 0C
			std::uintptr_t       unk10;     // 10
		};
		static_assert(sizeof(BoneNodePosition) == 0x18);

		// Laid out from Fallout4VR.exe 1.2.72 (GetBoneNode by name reads transforms at 0x188 and steps 0xA0 a
		// bone); the flat game is taken to keep the same layout after its smaller NiNode.
		struct RUNTIME_DATA
		{
#define RUNTIME_DATA_CONTENT                \
	std::int32_t      numTransforms; /*00*/ \
	std::uint32_t     pad04;         /*04*/ \
	BoneTransforms*   transforms;    /*08*/ \
	std::uint64_t     unk10;         /*10*/ \
	std::uint64_t     unk18;         /*18*/ \
	std::uint64_t     unk20;         /*20*/ \
	std::uint64_t     unk28;         /*28*/ \
	std::uint64_t     unk30;         /*30*/ \
	BoneNodePosition* bonePositions; /*38*/
			RUNTIME_DATA_CONTENT
		};
		static_assert(sizeof(RUNTIME_DATA) == 0x40);

		// The index of the named bone in transforms, or -1.
		[[nodiscard]] std::int32_t GetBoneIndex(const BSFixedString& a_name)
		{
			using func_t = decltype(&BSFlattenedBoneTree::GetBoneIndex);
			static REL::Relocation<func_t> func{ REL::RelocationID(906762, 0) };  // VR: tested
			return func(this, a_name);
		}

		// The named bone's node, or null.
		[[nodiscard]] NiNode* GetBoneNode(const BSFixedString& a_name)
		{
			using func_t = NiNode* (BSFlattenedBoneTree::*)(const BSFixedString&);
			static REL::Relocation<func_t> func{ REL::VariantID(67532, 0, 0x1C21070) };  // VR: tested
			return func(this, a_name);
		}

		// The node of the bone at a_index in transforms, or null.
		[[nodiscard]] NiNode* GetBoneNode(std::int32_t a_index)
		{
			using func_t = NiNode* (BSFlattenedBoneTree::*)(std::int32_t);
			static REL::Relocation<func_t> func{ REL::RelocationID(1477276, 0) };  // VR: tested
			return func(this, a_index);
		}

		// Copies the bones' nodes into transforms, so what was done to the nodes this frame is what gets skinned.
		void UpdateBoneArray()
		{
			using func_t = decltype(&BSFlattenedBoneTree::UpdateBoneArray);
			static REL::Relocation<func_t> func{ REL::RelocationID(1421730, 0) };  // VR: tested
			return func(this);
		}

		[[nodiscard]] inline RUNTIME_DATA& GetBoneTreeRuntimeData() noexcept
		{
			return REL::RelocateMember<RUNTIME_DATA>(this, 0x140, 0x180);
		}

		[[nodiscard]] inline const RUNTIME_DATA& GetBoneTreeRuntimeData() const noexcept
		{
			return REL::RelocateMember<RUNTIME_DATA>(this, 0x140, 0x180);
		}

		// members
#if !defined(ENABLE_FALLOUT_VR) || ((!defined(ENABLE_FALLOUT_NG) && !defined(ENABLE_FALLOUT_F4)))
		RUNTIME_DATA_CONTENT
#endif
	};
#if !defined(ENABLE_FALLOUT_VR)
	static_assert(sizeof(BSFlattenedBoneTree) == 0x180);
#elif (!defined(ENABLE_FALLOUT_NG) && !defined(ENABLE_FALLOUT_F4))
	static_assert(sizeof(BSFlattenedBoneTree) == 0x1C0);
#endif

	// The bone names of the human skeleton (skeleton.nif), power armor's included, as BSFlattenedBoneTree and
	// NiAVObject::GetObjectByName take them.
	namespace SkeletonBones
	{
		inline constexpr std::string_view COM{ "COM" };
		inline constexpr std::string_view Pelvis{ "Pelvis" };

		inline constexpr std::string_view LLeg_Thigh{ "LLeg_Thigh" };
		inline constexpr std::string_view LLeg_Calf{ "LLeg_Calf" };
		inline constexpr std::string_view LLeg_Foot{ "LLeg_Foot" };
		inline constexpr std::string_view LLeg_Toe1{ "LLeg_Toe1" };
		inline constexpr std::string_view LLeg_Calf_skin{ "LLeg_Calf_skin" };
		inline constexpr std::string_view LLeg_Calf_Low_skin{ "LLeg_Calf_Low_skin" };
		inline constexpr std::string_view LLeg_Calf_Armor1{ "LLeg_Calf_Armor1" };
		inline constexpr std::string_view LLeg_Calf_Armor2{ "LLeg_Calf_Armor2" };
		inline constexpr std::string_view LLeg_Thigh_skin{ "LLeg_Thigh_skin" };
		inline constexpr std::string_view LLeg_Thigh_Low_skin{ "LLeg_Thigh_Low_skin" };
		inline constexpr std::string_view LLeg_Thigh_Fat_skin{ "LLeg_Thigh_Fat_skin" };
		inline constexpr std::string_view LLeg_Thigh_Armor{ "LLeg_Thigh_Armor" };

		inline constexpr std::string_view RLeg_Thigh{ "RLeg_Thigh" };
		inline constexpr std::string_view RLeg_Calf{ "RLeg_Calf" };
		inline constexpr std::string_view RLeg_Foot{ "RLeg_Foot" };
		inline constexpr std::string_view RLeg_Toe1{ "RLeg_Toe1" };
		inline constexpr std::string_view RLeg_Calf_skin{ "RLeg_Calf_skin" };
		inline constexpr std::string_view RLeg_Calf_Low_skin{ "RLeg_Calf_Low_skin" };
		inline constexpr std::string_view RLeg_Calf_Armor1{ "RLeg_Calf_Armor1" };
		inline constexpr std::string_view RLeg_Calf_Armor2{ "RLeg_Calf_Armor2" };
		inline constexpr std::string_view RLeg_Thigh_skin{ "RLeg_Thigh_skin" };
		inline constexpr std::string_view RLeg_Thigh_Low_skin{ "RLeg_Thigh_Low_skin" };
		inline constexpr std::string_view RLeg_Thigh_Fat_skin{ "RLeg_Thigh_Fat_skin" };
		inline constexpr std::string_view RLeg_Thigh_Armor{ "RLeg_Thigh_Armor" };

		inline constexpr std::string_view Pelvis_skin{ "Pelvis_skin" };
		inline constexpr std::string_view RButtFat_skin{ "RButtFat_skin" };
		inline constexpr std::string_view LButtFat_skin{ "LButtFat_skin" };
		inline constexpr std::string_view Pelvis_Rear_skin{ "Pelvis_Rear_skin" };
		inline constexpr std::string_view Pelvis_Armor{ "Pelvis_Armor" };

		inline constexpr std::string_view SPINE1{ "SPINE1" };
		inline constexpr std::string_view SPINE2{ "SPINE2" };
		inline constexpr std::string_view Chest{ "Chest" };

		inline constexpr std::string_view LArm_Collarbone{ "LArm_Collarbone" };
		inline constexpr std::string_view LArm_UpperArm{ "LArm_UpperArm" };
		inline constexpr std::string_view LArm_ForeArm1{ "LArm_ForeArm1" };
		inline constexpr std::string_view LArm_ForeArm2{ "LArm_ForeArm2" };
		inline constexpr std::string_view LArm_ForeArm3{ "LArm_ForeArm3" };
		inline constexpr std::string_view LArm_Hand{ "LArm_Hand" };

		inline constexpr std::string_view LArm_Finger11{ "LArm_Finger11" };
		inline constexpr std::string_view LArm_Finger12{ "LArm_Finger12" };
		inline constexpr std::string_view LArm_Finger13{ "LArm_Finger13" };
		inline constexpr std::string_view LArm_Finger21{ "LArm_Finger21" };
		inline constexpr std::string_view LArm_Finger22{ "LArm_Finger22" };
		inline constexpr std::string_view LArm_Finger23{ "LArm_Finger23" };
		inline constexpr std::string_view LArm_Finger31{ "LArm_Finger31" };
		inline constexpr std::string_view LArm_Finger32{ "LArm_Finger32" };
		inline constexpr std::string_view LArm_Finger33{ "LArm_Finger33" };
		inline constexpr std::string_view LArm_Finger41{ "LArm_Finger41" };
		inline constexpr std::string_view LArm_Finger42{ "LArm_Finger42" };
		inline constexpr std::string_view LArm_Finger43{ "LArm_Finger43" };
		inline constexpr std::string_view LArm_Finger51{ "LArm_Finger51" };
		inline constexpr std::string_view LArm_Finger52{ "LArm_Finger52" };
		inline constexpr std::string_view LArm_Finger53{ "LArm_Finger53" };

		inline constexpr std::string_view WeaponLeft{ "WeaponLeft" };
		inline constexpr std::string_view AnimObjectL1{ "AnimObjectL1" };
		inline constexpr std::string_view AnimObjectL2{ "AnimObjectL2" };
		inline constexpr std::string_view AnimObjectL3{ "AnimObjectL3" };
		inline constexpr std::string_view PipboyBone{ "PipboyBone" };

		inline constexpr std::string_view LArm_ForeArm3_skin{ "LArm_ForeArm3_skin" };
		inline constexpr std::string_view LArm_ForeArm2_skin{ "LArm_ForeArm2_skin" };
		inline constexpr std::string_view LArm_ForeArm1_skin{ "LArm_ForeArm1_skin" };
		inline constexpr std::string_view LArm_ForeArm_Armor{ "LArm_ForeArm_Armor" };

		inline constexpr std::string_view LArm_UpperTwist1{ "LArm_UpperTwist1" };
		inline constexpr std::string_view LArm_UpperTwist2{ "LArm_UpperTwist2" };
		inline constexpr std::string_view LArm_UpperTwist2_skin{ "LArm_UpperTwist2_skin" };
		inline constexpr std::string_view LArm_UpperFat_skin{ "LArm_UpperFat_skin" };
		inline constexpr std::string_view LArm_UpperTwist1_skin{ "LArm_UpperTwist1_skin" };
		inline constexpr std::string_view LArm_UpperArm_skin{ "LArm_UpperArm_skin" };
		inline constexpr std::string_view LArm_UpperArm_Armor{ "LArm_UpperArm_Armor" };
		inline constexpr std::string_view LArm_Collarbone_skin{ "LArm_Collarbone_skin" };
		inline constexpr std::string_view LArm_ShoulderFat_skin{ "LArm_ShoulderFat_skin" };

		inline constexpr std::string_view Neck{ "Neck" };
		inline constexpr std::string_view Head{ "Head" };
		inline constexpr std::string_view Head_skin{ "Head_skin" };
		inline constexpr std::string_view Face_skin{ "Face_skin" };
		inline constexpr std::string_view Helmet_Armor{ "Helmet_Armor" };
		inline constexpr std::string_view Neck_skin{ "Neck_skin" };
		inline constexpr std::string_view Neck1_skin{ "Neck1_skin" };

		inline constexpr std::string_view RArm_Collarbone{ "RArm_Collarbone" };
		inline constexpr std::string_view RArm_UpperArm{ "RArm_UpperArm" };
		inline constexpr std::string_view RArm_ForeArm1{ "RArm_ForeArm1" };
		inline constexpr std::string_view RArm_ForeArm2{ "RArm_ForeArm2" };
		inline constexpr std::string_view RArm_ForeArm3{ "RArm_ForeArm3" };
		inline constexpr std::string_view RArm_Hand{ "RArm_Hand" };

		inline constexpr std::string_view RArm_Finger11{ "RArm_Finger11" };
		inline constexpr std::string_view RArm_Finger12{ "RArm_Finger12" };
		inline constexpr std::string_view RArm_Finger13{ "RArm_Finger13" };
		inline constexpr std::string_view RArm_Finger21{ "RArm_Finger21" };
		inline constexpr std::string_view RArm_Finger22{ "RArm_Finger22" };
		inline constexpr std::string_view RArm_Finger23{ "RArm_Finger23" };
		inline constexpr std::string_view RArm_Finger31{ "RArm_Finger31" };
		inline constexpr std::string_view RArm_Finger32{ "RArm_Finger32" };
		inline constexpr std::string_view RArm_Finger33{ "RArm_Finger33" };
		inline constexpr std::string_view RArm_Finger41{ "RArm_Finger41" };
		inline constexpr std::string_view RArm_Finger42{ "RArm_Finger42" };
		inline constexpr std::string_view RArm_Finger43{ "RArm_Finger43" };
		inline constexpr std::string_view RArm_Finger51{ "RArm_Finger51" };
		inline constexpr std::string_view RArm_Finger52{ "RArm_Finger52" };
		inline constexpr std::string_view RArm_Finger53{ "RArm_Finger53" };

		inline constexpr std::string_view Weapon{ "Weapon" };
		inline constexpr std::string_view AnimObjectR1{ "AnimObjectR1" };
		inline constexpr std::string_view AnimObjectR2{ "AnimObjectR2" };
		inline constexpr std::string_view AnimObjectR3{ "AnimObjectR3" };

		inline constexpr std::string_view RArm_ForeArm3_skin{ "RArm_ForeArm3_skin" };
		inline constexpr std::string_view RArm_ForeArm2_skin{ "RArm_ForeArm2_skin" };
		inline constexpr std::string_view RArm_ForeArm1_skin{ "RArm_ForeArm1_skin" };
		inline constexpr std::string_view RArm_ForeArm_Armor{ "RArm_ForeArm_Armor" };

		inline constexpr std::string_view RArm_UpperTwist1{ "RArm_UpperTwist1" };
		inline constexpr std::string_view RArm_UpperTwist2{ "RArm_UpperTwist2" };
		inline constexpr std::string_view RArm_UpperTwist2_skin{ "RArm_UpperTwist2_skin" };
		inline constexpr std::string_view RArm_UpperFat_skin{ "RArm_UpperFat_skin" };
		inline constexpr std::string_view RArm_UpperTwist1_skin{ "RArm_UpperTwist1_skin" };
		inline constexpr std::string_view RArm_UpperArm_skin{ "RArm_UpperArm_skin" };
		inline constexpr std::string_view RArm_UpperArm_Armor{ "RArm_UpperArm_Armor" };
		inline constexpr std::string_view RArm_Collarbone_skin{ "RArm_Collarbone_skin" };
		inline constexpr std::string_view RArm_ShoulderFat_skin{ "RArm_ShoulderFat_skin" };

		inline constexpr std::string_view Chest_skin{ "Chest_skin" };
		inline constexpr std::string_view LBreast_skin{ "LBreast_skin" };
		inline constexpr std::string_view RBreast_skin{ "RBreast_skin" };
		inline constexpr std::string_view Chest_Rear_Skin{ "Chest_Rear_Skin" };
		inline constexpr std::string_view Chest_Upper_skin{ "Chest_Upper_skin" };
		inline constexpr std::string_view Neck_Low_skin{ "Neck_Low_skin" };

		inline constexpr std::string_view Back_Armor{ "Back_Armor" };
		inline constexpr std::string_view Tank_Armor{ "Tank_Armor" };
		inline constexpr std::string_view Wheel{ "Wheel" };
		inline constexpr std::string_view ProjectileNode{ "ProjectileNode" };

		inline constexpr std::string_view Pauldron_Armor{ "Pauldron_Armor" };
		inline constexpr std::string_view L_Pauldron{ "L_Pauldron" };
		inline constexpr std::string_view R_Pauldron{ "R_Pauldron" };

		inline constexpr std::string_view Spine2_skin{ "Spine2_skin" };
		inline constexpr std::string_view UpperBelly_skin{ "UpperBelly_skin" };
		inline constexpr std::string_view Spine2_Rear_skin{ "Spine2_Rear_skin" };
		inline constexpr std::string_view Spine1_skin{ "Spine1_skin" };
		inline constexpr std::string_view Belly_skin{ "Belly_skin" };
		inline constexpr std::string_view Spine1_Rear_skin{ "Spine1_Rear_skin" };

		inline constexpr std::string_view Camera{ "Camera" };
		inline constexpr std::string_view Camera_Control{ "Camera Control" };
		inline constexpr std::string_view AnimObjectA{ "AnimObjectA" };
		inline constexpr std::string_view AnimObjectB{ "AnimObjectB" };
		inline constexpr std::string_view CamTargetParent{ "CamTargetParent" };
		inline constexpr std::string_view CamTarget{ "CamTarget" };
		inline constexpr std::string_view OpenArmor{ "OpenArmor" };

		// not in the power armor skeleton
		inline constexpr std::string_view L_RibHelper{ "L_RibHelper" };
		inline constexpr std::string_view R_RibHelper{ "R_RibHelper" };
	}
}
#undef RUNTIME_DATA_CONTENT
