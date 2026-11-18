#include "REL/Guarded.h"

#include "REL/Module.h"

#include <excpt.h>

namespace REL
{
	namespace
	{
		// Windows never maps the first 64 KiB, so a null pointer plus a field offset lands here.
		// Rejecting it up front avoids raising an exception for the most common bad pointer.
		constexpr std::uintptr_t kLowestMappedAddress = 0x10000;

		// Largest vtable index accepted. Real tables are far shorter; the bound only keeps a
		// garbage index from turning into a read far past the table.
		constexpr std::size_t kMaxVtableIndex = 0x400;

		[[nodiscard]] bool mappable(const void* a_pointer) noexcept
		{
			return reinterpret_cast<std::uintptr_t>(a_pointer) >= kLowestMappedAddress;
		}

		// No objects with destructors may live in a function that uses __try.
		[[nodiscard]] bool copy_under_seh(const void* a_source, void* a_destination, std::size_t a_size) noexcept
		{
			__try {
				std::memcpy(a_destination, a_source, a_size);
				return true;
			} __except (EXCEPTION_EXECUTE_HANDLER) {
				return false;
			}
		}

		void call_under_seh(void (*a_function)(void*) noexcept, void* a_context, GuardedCallResult& a_result) noexcept
		{
			__try {
				a_function(a_context);
				a_result.completed = true;
			} __except (a_result.exceptionCode = GetExceptionCode(), EXCEPTION_EXECUTE_HANDLER) {
				a_result.completed = false;
			}
		}
	}

	bool guarded_copy(const void* a_source, void* a_destination, std::size_t a_size) noexcept
	{
		if (a_size == 0 || !mappable(a_source) || !mappable(a_destination)) {
			return false;
		}
		return copy_under_seh(a_source, a_destination, a_size);
	}

	std::uintptr_t guarded_read_vfunc(const void* a_object, std::size_t a_index) noexcept
	{
		if (a_index > kMaxVtableIndex) {
			return 0;
		}
		const auto table = guarded_read<std::uintptr_t>(a_object);
		if (!table || *table < kLowestMappedAddress) {
			return 0;
		}
		return guarded_read<std::uintptr_t>(*table + a_index * sizeof(void*)).value_or(0);
	}

	GuardedCallResult guarded_call(void (*a_function)(void*) noexcept, void* a_context) noexcept
	{
		GuardedCallResult result{};
		if (a_function) {
			call_under_seh(a_function, a_context, result);
		}
		return result;
	}

	bool code_matches(std::uintptr_t a_address, std::span<const std::uint8_t> a_expected) noexcept
	{
		if (a_expected.empty()) {
			return false;
		}
		const auto text = Module::get().segment(Segment::text);
		const auto begin = text.address();
		const auto end = begin + text.size();
		if (a_address < begin || a_address > end || a_expected.size() > end - a_address) {
			return false;
		}
		return std::memcmp(reinterpret_cast<const void*>(a_address), a_expected.data(), a_expected.size()) == 0;
	}
}
