#pragma once

// hknpDynamicCompoundShape — a compound shape whose children can move at runtime
//
// A compound shape is a list of child shapes (hknpShapeInstance), each placed by its own
// transform inside the parent body. The dynamic variant lets you move the children after the
// shape is built: change instance transforms with hknpShapeInstance::SetTransform, then hand
// them to UpdateInstances. Articulated furniture such as a power armor frame uses one for its
// collision.
//
// Usage (replace a body's compound with a copy whose children you can move):
//   std::array<RE::hknpShapeInstance, N> children = ...;     // copied from the original
//   std::array<RE::hknpShapeInstanceId, N> ids{};
//   RE::hknpCompoundShapeCinfo cinfo{ children.data(), N };
//   cinfo.instanceIdsOut = ids.data();
//   auto* shape = RE::hknpDynamicCompoundShape::Create(cinfo);  // holds one reference
//   ...
//   shape->UpdateInstances(ids.data(), N, movedChildren.data());
//   shape->RemoveReference();                                  // when you no longer need it
//
// Layout and functions verified in Fallout4VR.exe 1.2.72: constructor 0x1416E42B0 and
// destructor 0x1416E4450 (vtable 0x142E017C8, last member at +0xC0), compound base
// constructor 0x1416E2BE0 (instance array at +0x60, size +0x68, flags +0x6C, free list +0x70).

#include "RE/Havok/hkMemoryAllocator.h"
#include "RE/Havok/hkMemoryRouter.h"
#include "RE/Havok/hknpShape.h"
#include "RE/Havok/hknpShapeInstance.h"

namespace RE
{
	/// Construction info for a compound shape.
	struct hknpCompoundShapeCinfo
	{
	public:
		static constexpr REL::ID ConstructorID{ 1066123 };  // FO4VR 0x1416E1CF0

		/// True when FO4VR's constructor starts with the expected code.
		[[nodiscard]] static bool VRCodeMatches()
		{
			static constexpr std::uint8_t code[]{ 0x33, 0xC0, 0x48, 0x89, 0x11, 0x44, 0x89, 0x41 };
			return REL::code_matches(ConstructorID.address(), code);
		}

		/// a_massConfig is an optional hknpShape::MassConfig*; nullptr builds no mass properties.
		hknpCompoundShapeCinfo(const hknpShapeInstance* a_instances, std::int32_t a_numInstances, void* a_massConfig = nullptr)
		{
			using func_t = hknpCompoundShapeCinfo* (*)(hknpCompoundShapeCinfo*, const hknpShapeInstance*, std::int32_t, void*);
			static REL::Relocation<func_t> func{ ConstructorID };
			func(this, a_instances, a_numInstances, a_massConfig);
		}

		// members
		const hknpShapeInstance* instances;         // 00
		std::int32_t             numInstances;      // 08
		std::int32_t             instanceCapacity;  // 0C - the shape reserves max(numInstances, instanceCapacity)
		bool                     unk10;             // 10 - copied to the shape at +0xA0
		std::uint8_t             pad11[7];          // 11
		void*                    massConfig;        // 18 - hknpShape::MassConfig*
		hknpShapeInstanceId*     instanceIdsOut;    // 20 - receives each instance's id, in order; may be nullptr
	};
	static_assert(sizeof(hknpCompoundShapeCinfo) == 0x28);

	class __declspec(novtable) hknpDynamicCompoundShape :
		public hknpShape  // 00
	{
	public:
		static constexpr auto RTTI{ RTTI::hknpDynamicCompoundShape };
		static constexpr auto VTABLE{ VTABLE::hknpDynamicCompoundShape };

		static constexpr REL::ID ConstructorID{ 1316944 };      // FO4VR 0x1416E42B0
		static constexpr REL::ID UpdateInstancesID{ 1248513 };  // FO4VR 0x1416E4850

		/// True when FO4VR's constructor and UpdateInstances start with the expected code.
		[[nodiscard]] static bool VRCodeMatches()
		{
			static constexpr std::uint8_t constructor[]{ 0x48, 0x89, 0x5C, 0x24, 0x10, 0x48, 0x89, 0x74 };
			static constexpr std::uint8_t updateInstances[]{ 0x48, 0x89, 0x5C, 0x24, 0x18, 0x55, 0x41, 0x54 };
			return REL::code_matches(ConstructorID.address(), constructor) &&
			       REL::code_matches(UpdateInstancesID.address(), updateInstances);
		}

		/// Allocates a shape from the Havok heap of the calling thread and builds it from a_cinfo.
		/// Returns nullptr when the thread has no Havok heap or the allocation fails.
		/// The new shape holds one reference: release it with RemoveReference.
		[[nodiscard]] static hknpDynamicCompoundShape* Create(const hknpCompoundShapeCinfo& a_cinfo)
		{
			auto* router = hkMemoryRouter::GetInstancePtr();
			if (!router || !router->heap) {
				return nullptr;
			}
			void* storage = router->heap->BlockAlloc(static_cast<std::int32_t>(sizeof(hknpDynamicCompoundShape)));
			if (!storage) {
				return nullptr;
			}
			std::memset(storage, 0, sizeof(hknpDynamicCompoundShape));

			using func_t = hknpDynamicCompoundShape* (*)(void*, const hknpCompoundShapeCinfo&);
			static REL::Relocation<func_t> func{ ConstructorID };
			return func(storage, a_cinfo);
		}

		/// Replaces the instances named by a_ids with the records in a_instances (usually copies
		/// moved with SetTransform). Havok's updateInstances.
		void UpdateInstances(const hknpShapeInstanceId* a_ids, std::int32_t a_count, const hknpShapeInstance* a_instances)
		{
			using func_t = void (*)(hknpDynamicCompoundShape*, const hknpShapeInstanceId*, std::int32_t, const hknpShapeInstance*);
			static REL::Relocation<func_t> func{ UpdateInstancesID };
			func(this, a_ids, a_count, a_instances);
		}

		// members
		std::byte          pad30[0x30];               // 30 - composite and compound shape data, not mapped
		hknpShapeInstance* instances;                 // 60 - instance array, indexed by hknpShapeInstanceId
		std::int32_t       numInstances;              // 68 - array size, free slots included
		std::int32_t       instanceCapacityAndFlags;  // 6C
		std::int32_t       firstFreeInstance;         // 70 - head of the free-slot list, -1 when none
		std::byte          pad74[0x5C];               // 74 - bounding volume and dynamic-tree data, not mapped
	};
	static_assert(sizeof(hknpDynamicCompoundShape) == 0xD0);
}
