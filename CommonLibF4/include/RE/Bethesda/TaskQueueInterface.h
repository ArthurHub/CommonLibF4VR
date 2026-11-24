#pragma once

namespace RE
{
	enum class DEFAULT_OBJECT;

	class BSPackedTaskQueue;
	class NiAVObject;
	class TESAmmo;
	class TESObjectREFR;
	class TESObjectWEAP;

	class TaskQueueInterface
	{
	public:
		static TaskQueueInterface* GetSingleton()
		{
			static REL::Relocation<TaskQueueInterface**> singleton{ REL::ID(7491) };  // VR: tested
			return *singleton;
		}

		// Queues the combatant starting combat with the target, applied on the engine's schedule a frame or more
		// later rather than at once; a_commandAlly as Papyrus Actor.StartCombat's abCommandAlly.
		void QueueActorStartCombat(Actor& a_combatant, Actor& a_target, bool a_commandAlly)
		{
			using func_t = decltype(&TaskQueueInterface::QueueActorStartCombat);
			static REL::Relocation<func_t> func{ REL::RelocationID(179777, 0) };  // VR: tested
			return func(this, a_combatant, a_target, a_commandAlly);
		}

		void QueueClosePipboy()
		{
			using func_t = decltype(&TaskQueueInterface::QueueClosePipboy);
			static REL::Relocation<func_t> func{ REL::VariantID(69886, 0, 0xDAEFD0) };
			return func(this);
		}

		void QueueRebuildBendableSpline(TESObjectREFR& a_ref, bool a_createPhysics, NiAVObject* a_childNode)
		{
			using func_t = decltype(&TaskQueueInterface::QueueRebuildBendableSpline);
			static REL::Relocation<func_t> func{ REL::ID(198419) };
			return func(this, a_ref, a_createPhysics, a_childNode);
		}

		void QueueShow1stPerson(bool a_show1stPerson)
		{
			using func_t = decltype(&TaskQueueInterface::QueueShow1stPerson);
			static REL::Relocation<func_t> func{ REL::VariantID(994377, 0, 0xDADDB0) };  // VR: checked
			return func(this, a_show1stPerson);
		}

		void QueueShowPipboy(DEFAULT_OBJECT a_animType)
		{
			using func_t = decltype(&TaskQueueInterface::QueueShowPipboy);
			static REL::Relocation<func_t> func{ REL::VariantID(1133684, 0, 0xDAEF10) };  // VR: checked
			return func(this, a_animType);
		}

		void QueueUpdate3D(TESObjectREFR* a_ref, std::uint32_t a_flags)
		{
			using func_t = decltype(&TaskQueueInterface::QueueUpdate3D);
			static REL::Relocation<func_t> func{ REL::ID(581890) };
			return func(this, a_ref, a_flags);
		}

		void QueueWeaponFire(TESObjectWEAP* a_weapon, TESObjectREFR* a_refObject, BGSEquipIndex a_equipIndex, TESAmmo* a_ammo)
		{
			using func_t = decltype(&TaskQueueInterface::QueueWeaponFire);
			static REL::Relocation<func_t> func{ REL::ID(15449) };
			return func(this, a_weapon, a_refObject, a_equipIndex, a_ammo);
		}

		// members
		BSPackedTaskQueue* queue;           // 00
		BSPackedTaskQueue* secondaryQueue;  // 08
		BSPackedTaskQueue* activeQueue;     // 18
	};
	static_assert(sizeof(TaskQueueInterface) == 0x18);
}
