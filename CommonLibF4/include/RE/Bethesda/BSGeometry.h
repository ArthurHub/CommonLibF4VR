#pragma once

#include "RE/Bethesda/BSGraphics.h"
#include "RE/NetImmerse/NiAVObject.h"
#include "RE/NetImmerse/NiProperty.h"

namespace RE
{
	class BSCombinedTriShape;
	class BSGeometrySegmentData;
	class BSMergeInstancedTriShape;
	class BSMultiIndexTriShape;

	namespace BSGraphics
	{
		struct IndexBuffer;
	}

	namespace BSSkin
	{
		class Instance;
	}

	class __declspec(novtable) BSGeometry :
		public NiAVObject  // 000
	{
	public:
		static constexpr auto RTTI{ RTTI::BSGeometry };
		static constexpr auto VTABLE{ VTABLE::BSGeometry };
		static constexpr auto Ni_RTTI{ Ni_RTTI::BSGeometry };

		// Address Library ID of BSGeometry's UpdateWorldData (FO4VR 0x141C31C10).
		// See NiAVObject::VR_UpdateWorldDataSlot.
		static constexpr REL::ID UpdateWorldDataID{ 551661 };

		/// True when FO4VR's BSGeometry UpdateWorldData starts with the expected code.
		[[nodiscard]] static bool VRCodeMatches()
		{
			static constexpr std::uint8_t code[]{ 0x40, 0x53, 0x48, 0x83, 0xEC, 0x20 };
			return REL::code_matches(UpdateWorldDataID.address(), code);
		}

		BSGeometry();
		virtual ~BSGeometry();  // NOLINT(modernize-use-override) 00

		// add
		void UpdatePropertyControllers(NiUpdateData& a_data)  // 3A
		{
			REL::RelocateVirtual<decltype(&BSGeometry::UpdatePropertyControllers)>(0x3A, 0x3D, this, a_data);
		}

		BSGeometrySegmentData* GetSegmentData()  // 3B
		{
			return REL::RelocateVirtual<decltype(&BSGeometry::GetSegmentData)>(0x3B, 0x3E, this);
		}

		void SetSegmentData(BSGeometrySegmentData* a_data)  // 3C
		{
			REL::RelocateVirtual<decltype(&BSGeometry::SetSegmentData)>(0x3C, 0x3F, this, a_data);
		}

		BSGraphics::IndexBuffer* GetCustomIndexBuffer()  // 3D
		{
			return REL::RelocateVirtual<decltype(&BSGeometry::GetCustomIndexBuffer)>(0x3D, 0x40, this);
		}

		BSCombinedTriShape* IsBSCombinedTriShape()  // 3E
		{
			return REL::RelocateVirtual<decltype(&BSGeometry::IsBSCombinedTriShape)>(0x3E, 0x41, this);
		}

		BSMergeInstancedTriShape* IsBSMergeInstancedTriShape()  // 3F
		{
			return REL::RelocateVirtual<decltype(&BSGeometry::IsBSMergeInstancedTriShape)>(0x3F, 0x42, this);
		}

		BSMultiIndexTriShape* IsMultiIndexTriShape()  // 40
		{
			return REL::RelocateVirtual<decltype(&BSGeometry::IsMultiIndexTriShape)>(0x40, 0x43, this);
		}

		std::uint32_t GetRenderableTris(std::uint32_t a_LODMode)  // 41
		{
			return REL::RelocateVirtual<decltype(&BSGeometry::GetRenderableTris)>(0x41, 0x44, this, a_LODMode);
		}

		struct RUNTIME_DATA
		{
#define BSGEOMETRY_RUNTIME_DATA_CONTENT                                    \
	NiBound                     modelBound;    /* 120 (flat) / 160 (VR) */ \
	NiPointer<NiProperty>       properties[2]; /* 130 (flat) / 170 (VR) */ \
	NiPointer<BSSkin::Instance> skinInstance;  /* 140 (flat) / 180 (VR) */ \
	void*                       rendererData;  /* 148 (flat) / 188 (VR) */ \
	BSGraphics::VertexDesc      vertexDesc;    /* 150 (flat) / 190 (VR) */ \
	std::uint8_t                type;          /* 158 (flat) / 198 (VR) */ \
	bool                        registered;    /* 159 (flat) / 199 (VR) */
            BSGEOMETRY_RUNTIME_DATA_CONTENT
		};
		static_assert(sizeof(RUNTIME_DATA) == 0x40);

		[[nodiscard]] inline RUNTIME_DATA& GetRuntimeData() noexcept
		{
			return REL::RelocateMember<RUNTIME_DATA>(this, 0x120, 0x160);
		}

		[[nodiscard]] inline const RUNTIME_DATA& GetRuntimeData() const noexcept
		{
			return REL::RelocateMember<RUNTIME_DATA>(this, 0x120, 0x160);
		}

		// members
#if !defined(ENABLE_FALLOUT_VR)
		BSGEOMETRY_RUNTIME_DATA_CONTENT
#elif (!defined(ENABLE_FALLOUT_NG) && !defined(ENABLE_FALLOUT_F4))
		std::uint8_t pad120[0x40];  // 120
		BSGEOMETRY_RUNTIME_DATA_CONTENT
#endif
	};
#if !defined(ENABLE_FALLOUT_VR)
	static_assert(sizeof(BSGeometry) == 0x160);
#elif (!defined(ENABLE_FALLOUT_NG) && !defined(ENABLE_FALLOUT_F4))
	static_assert(sizeof(BSGeometry) == 0x1A0);
#endif

	class __declspec(novtable) BSTriShape :
		public BSGeometry  // 000
	{
	public:
		static constexpr auto RTTI{ RTTI::BSTriShape };
		static constexpr auto VTABLE{ VTABLE::BSTriShape };
		static constexpr auto Ni_RTTI{ Ni_RTTI::BSTriShape };

		BSTriShape();
		~BSTriShape() override;  // 00

		// override (BSGeometry)
		const NiRTTI* GetRTTI() const override  // 02
		{
			static REL::Relocation<const NiRTTI*> rtti{ BSTriShape::Ni_RTTI };
			return rtti.get();
		}

		BSTriShape* IsTriShape() override { return this; }  // 0A

		struct TRISHAPE_RUNTIME_DATA
		{
#define BSTRISHAPE_RUNTIME_DATA_CONTENT                     \
	std::uint32_t numTriangles; /* 160 (flat) / 1A0 (VR) */ \
	std::uint16_t numVertices;  /* 164 (flat) / 1A4 (VR) */
			BSTRISHAPE_RUNTIME_DATA_CONTENT
		};
		static_assert(sizeof(TRISHAPE_RUNTIME_DATA) == 0x8);

		[[nodiscard]] inline TRISHAPE_RUNTIME_DATA& GetTriShapeRuntimeData() noexcept
		{
			return REL::RelocateMember<TRISHAPE_RUNTIME_DATA>(this, 0x160, 0x1A0);
		}

		[[nodiscard]] inline const TRISHAPE_RUNTIME_DATA& GetTriShapeRuntimeData() const noexcept
		{
			return REL::RelocateMember<TRISHAPE_RUNTIME_DATA>(this, 0x160, 0x1A0);
		}

		// members
		// Note: no VR pad needed here — the +0x40 gap is already absorbed by BSGeometry
		// being 0x40 larger in VR (0x1A0 vs 0x160), so BSTriShape members follow at
		// 0x1A0 in VR and 0x160 in flat with no additional padding in this class.
		BSTRISHAPE_RUNTIME_DATA_CONTENT
	};
#if !defined(ENABLE_FALLOUT_VR)
	static_assert(sizeof(BSTriShape) == 0x170);
#elif (!defined(ENABLE_FALLOUT_NG) && !defined(ENABLE_FALLOUT_F4))
	static_assert(sizeof(BSTriShape) == 0x1B0);
#endif
}

#undef BSGEOMETRY_RUNTIME_DATA_CONTENT
#undef BSTRISHAPE_RUNTIME_DATA_CONTENT
