#pragma once

#include "RE/NetImmerse/NiObject.h"

namespace RE
{
	class NiUpdateData;

	class __declspec(novtable) NiCollisionObject :
		public NiObject  // 00
	{
	public:
		static constexpr auto RTTI{ RTTI::NiCollisionObject };
		static constexpr auto VTABLE{ VTABLE::NiCollisionObject };
		static constexpr auto Ni_RTTI{ Ni_RTTI::NiCollisionObject };

		// FO4VR slot of UpdateWorldData (flat 0x29). NiAVObject::UpdateWorldData
		// (FO4VR 0x141C23740) jumps through it when it gets update data and the object has a
		// collision object; with null update data the collision object is not updated.
		static constexpr std::size_t VR_UpdateWorldDataSlot{ 0x2B };

		// add
		virtual void SetSceneGraphObject(NiAVObject* a_sceneObject);                                               // 28
		virtual void UpdateWorldData(NiUpdateData& a_updateData);                                                  // 29
		virtual void Initialize([[maybe_unused]] void* a_data) { return; }                                         // 2A
		virtual void Convert([[maybe_unused]] std::uint32_t a_version, [[maybe_unused]] void* a_data) { return; }  // 2B

		// members
		NiAVObject* sceneObject;  // 10
	};
	static_assert(sizeof(NiCollisionObject) == 0x18);
}
