#pragma once

// hknpBSWorld — Bethesda's subclass of hknpWorld
//
// The hknpWorld that a bhkWorld wraps (cell->GetbhkWorld(), then FromBhkWorld) is an
// hknpBSWorld.
// Bethesda adds a read/write lock to it; hold the write lock while changing bodies or
// shapes from outside the physics step, as the engine's own Papyrus SetMotionType worker
// does around its motion change.
//
// Usage:
//   auto* world = RE::hknpBSWorld::FromBhkWorld(cell->GetbhkWorld());
//   if (world) {
//       RE::hknpBSWorld::WriteLock lock{ *world };
//       world->SetBodyShape(bodyId, newShape);
//   }

#include "RE/Havok/hknpWorld.h"

namespace RE
{
	class bhkWorld;

	class hknpBSWorld :
		public hknpWorld
	{
	public:
		static constexpr REL::ID MarkForWriteID{ 1017544 };   // FO4VR 0x141DF5FB0
		static constexpr REL::ID UnmarkForWriteID{ 312883 };  // FO4VR 0x141DF5FD0

		/// Holds the world's write lock for the lifetime of the object.
		class WriteLock
		{
		public:
			explicit WriteLock(hknpBSWorld& a_world) :
				_world(a_world)
			{
				_world.MarkForWrite();
			}

			~WriteLock() { _world.UnmarkForWrite(); }

			WriteLock(const WriteLock&) = delete;
			WriteLock& operator=(const WriteLock&) = delete;

		private:
			hknpBSWorld& _world;
		};

		/// The Havok world of a bhkWorld, or nullptr when a_bhkWorld is null, unreadable or has
		/// none. bhkWorld+0x60 holds it (bhkPhysicsSystem::HasInstanceInWorld compares the
		/// instance world with this field). The read is guarded (REL::guarded_read).
		[[nodiscard]] static hknpBSWorld* FromBhkWorld(const bhkWorld* a_bhkWorld) noexcept
		{
			if (!a_bhkWorld) {
				return nullptr;
			}
			return REL::guarded_read<hknpBSWorld*>(reinterpret_cast<std::uintptr_t>(a_bhkWorld) + 0x60).value_or(nullptr);
		}

		/// True when FO4VR's MarkForWrite and UnmarkForWrite start with the expected code.
		/// Both are `add rcx, 0x6D8; jmp` into the lock functions.
		[[nodiscard]] static bool VRCodeMatches()
		{
			static constexpr std::uint8_t code[]{ 0x48, 0x81, 0xC1, 0xD8, 0x06, 0x00, 0x00, 0xE9 };
			return REL::code_matches(MarkForWriteID.address(), code) &&
			       REL::code_matches(UnmarkForWriteID.address(), code);
		}

		/// Takes the write lock (the BSReadWriteLock at +0x6D8). Prefer WriteLock.
		void MarkForWrite()
		{
			using func_t = void (*)(hknpBSWorld*);
			static REL::Relocation<func_t> func{ MarkForWriteID };
			func(this);
		}

		/// Releases the write lock taken by MarkForWrite.
		void UnmarkForWrite()
		{
			using func_t = void (*)(hknpBSWorld*);
			static REL::Relocation<func_t> func{ UnmarkForWriteID };
			func(this);
		}
	};
}
