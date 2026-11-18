#pragma once

#include "RE/Havok/hkBaseObject.h"

namespace RE
{
	class hkClass;

	class __declspec(novtable) hkReferencedObject :
		public hkBaseObject  // 00
	{
	public:
		static constexpr auto RTTI{ RTTI::hkReferencedObject };
		static constexpr auto VTABLE{ VTABLE::hkReferencedObject };

		// add
		virtual const hkClass* GetClassType() const { return nullptr; }             // 02
		virtual void           DeleteThisReferencedObject() const { delete this; }  // 03

		// Reference counting, as the engine does it inline (for example in
		// hknpWorld::setBodyShape, FO4VR 0x14153AC70): the low 16 bits of the word at +0x08
		// are the reference count, the high 16 bits the memory size. Objects whose memory
		// size is 0 are not reference counted. Unlike the engine, a count that is already 0
		// or 0xFFFF is left alone instead of wrapping around.

		[[nodiscard]] std::uint16_t GetReferenceCount() const noexcept { return static_cast<std::uint16_t>(memSizeAndRefCount); }
		[[nodiscard]] std::uint16_t GetMemorySize() const noexcept { return static_cast<std::uint16_t>(memSizeAndRefCount >> 16); }

		void AddReference() const noexcept
		{
			auto* word = const_cast<volatile std::uint32_t*>(std::addressof(memSizeAndRefCount));
			for (;;) {
				const std::uint32_t before = *word;
				const auto          count = static_cast<std::uint16_t>(before);
				if ((before >> 16) == 0 || count == 0xFFFF) {
					return;
				}
				const std::uint32_t after = (before & 0xFFFF0000) | static_cast<std::uint16_t>(count + 1);
				if (REX::W32::InterlockedCompareExchange(word, after, before) == before) {
					return;
				}
			}
		}

		/// Deletes the object through DeleteThisReferencedObject when the count reaches 0.
		void RemoveReference() const noexcept
		{
			auto* word = const_cast<volatile std::uint32_t*>(std::addressof(memSizeAndRefCount));
			for (;;) {
				const std::uint32_t before = *word;
				const auto          count = static_cast<std::uint16_t>(before);
				if ((before >> 16) == 0 || count == 0 || count == 0xFFFF) {
					return;
				}
				const std::uint32_t after = (before & 0xFFFF0000) | static_cast<std::uint16_t>(count - 1);
				if (REX::W32::InterlockedCompareExchange(word, after, before) != before) {
					continue;
				}
				if (count == 1) {
					DeleteThisReferencedObject();
				}
				return;
			}
		}

		// members
		std::uint32_t memSizeAndRefCount;  // 08
	};
	static_assert(sizeof(hkReferencedObject) == 0x10);
}
