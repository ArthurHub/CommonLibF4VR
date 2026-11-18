#pragma once

// hknpShapeInstance — one child shape of a compound shape, placed by a transform
//
// Compound shapes (hknpDynamicCompoundShape and others) store their children as an array of
// these 0x80-byte records. The first 0x40 bytes hold the child's transform; Havok keeps
// flag bits in the w components of that transform, so change it with SetTransform rather
// than by writing the floats directly.

namespace RE
{
	class hkTransformf;

	struct alignas(0x10) hknpShapeInstance
	{
	public:
		static constexpr REL::ID SetTransformID{ 1387039 };  // FO4VR 0x1416E1840

		/// True when FO4VR's SetTransform starts with the expected code.
		[[nodiscard]] static bool VRCodeMatches()
		{
			static constexpr std::uint8_t code[]{ 0x48, 0x89, 0x5C, 0x24, 0x08, 0x48, 0x89, 0x6C };
			return REL::code_matches(SetTransformID.address(), code);
		}

		/// Sets the child transform and refreshes the flag bits kept in its w components.
		/// a_transform must be 16-byte aligned (the engine copies it with aligned SSE moves).
		void SetTransform(const hkTransformf& a_transform)
		{
			using func_t = void (*)(hknpShapeInstance*, const hkTransformf&);
			static REL::Relocation<func_t> func{ SetTransformID };
			func(this, a_transform);
		}

		// members
		std::byte transform[0x40];  // 00 - rotation columns and translation; w components hold flags
		std::byte data[0x40];       // 40 - shape pointer and per-instance data; +0x5C links free slots
	};
	static_assert(sizeof(hknpShapeInstance) == 0x80);

	/// Index of an instance inside its compound shape (Havok's hknpShapeInstanceId).
	using hknpShapeInstanceId = std::int16_t;
}
