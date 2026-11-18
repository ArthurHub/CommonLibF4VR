#pragma once

// Fault-tolerant access to game memory.
//
// An engine pointer seen during a frame can be stale (the object was freed by another
// system) or can hold something other than what the caller expects. These helpers turn the
// resulting access violation into a failed result instead of a crash.
//
// They rely on structured exception handling (SEH) alone and never call VirtualQuery.
// VirtualQuery walks the process address space and its cost grows with the size of the
// queried region, which makes it far too slow for per-frame checks. A guarded read that
// succeeds costs about as much as a plain memcpy.
//
// A successful read proves only that the memory was readable, not that it holds the object
// the caller expects: validate what you read (vtable identity, ranges, back-pointers).

namespace REL
{
	/// Copies a_size bytes from a_source to a_destination.
	/// Returns false, without crashing, if either range faults or a pointer is null or lies in
	/// the first 64 KiB of the address space, which Windows never maps.
	[[nodiscard]] bool guarded_copy(const void* a_source, void* a_destination, std::size_t a_size) noexcept;

	/// Reads a T at a_address. Returns std::nullopt if the read faults.
	template <class T>
		requires std::is_trivially_copyable_v<T>
	[[nodiscard]] std::optional<T> guarded_read(const void* a_address) noexcept
	{
		T value;
		if (!guarded_copy(a_address, std::addressof(value), sizeof(T))) {
			return std::nullopt;
		}
		return value;
	}

	template <class T>
		requires std::is_trivially_copyable_v<T>
	[[nodiscard]] std::optional<T> guarded_read(std::uintptr_t a_address) noexcept
	{
		return guarded_read<T>(reinterpret_cast<const void*>(a_address));
	}

	/// Reads entry a_index of a_object's virtual function table.
	/// Returns 0 if the object or its table cannot be read.
	/// The result is an address to compare with known functions. Do not call it unless it
	/// matches one: an unknown override may expect a different object or arguments.
	[[nodiscard]] std::uintptr_t guarded_read_vfunc(const void* a_object, std::size_t a_index) noexcept;

	struct GuardedCallResult
	{
		bool          completed{ false };  // false when a_function raised a hardware exception
		std::uint32_t exceptionCode{ 0 };  // the exception code when it did, e.g. 0xC0000005
	};

	/// Runs a_function(a_context) and stops it, instead of crashing, if it raises a hardware
	/// exception such as an access violation.
	///
	/// Keep a_function to plain reads and writes of engine memory and engine calls. When a
	/// fault stops it, objects with destructors inside it are not destroyed (C++ unwinding does
	/// not run for hardware exceptions under /EHsc), and engine state it already changed stays
	/// changed.
	[[nodiscard]] GuardedCallResult guarded_call(void (*a_function)(void*) noexcept, void* a_context) noexcept;

	/// True when the bytes at a_address equal a_expected.
	/// Use it to check that a hardcoded game address still holds the expected code before
	/// calling or comparing it. Addresses outside the game executable's .text segment return
	/// false without being read.
	[[nodiscard]] bool code_matches(std::uintptr_t a_address, std::span<const std::uint8_t> a_expected) noexcept;
}
