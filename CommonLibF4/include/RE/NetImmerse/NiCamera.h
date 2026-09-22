#pragma once

#include "RE/Bethesda/BSTArray.h"
#include "RE/NetImmerse/NiAVObject.h"
#include "RE/NetImmerse/NiFrustum.h"
#include "RE/NetImmerse/NiMatrix3.h"
#include "RE/NetImmerse/NiPoint.h"
#include "RE/NetImmerse/NiRect.h"

namespace RE
{
	class __declspec(novtable) NiCamera :
		public NiAVObject  // 00
	{
	public:
		static constexpr auto RTTI{ RTTI::NiCamera };
		static constexpr auto VTABLE{ VTABLE::NiCamera };
		static constexpr auto Ni_RTTI{ Ni_RTTI::NiCamera };

		virtual ~NiCamera();  // 00

		// override
		virtual const NiRTTI* GetRTTI() const override;                             // 02
		virtual NiObject*     CreateClone(NiCloningProcess& a_cloneData) override;  // 1A
		virtual void          LoadBinary(NiStream& a_stream) override;              // 1B
		virtual void          LinkObject(NiStream& a_stream) override;              // 1C
		virtual bool          RegisterStreamables(NiStream& a_stream) override;     // 1D
		virtual void          SaveBinary(NiStream& a_stream) override;              // 1E
		virtual bool          IsEqual(NiObject* a_object) override;                 // 1F

		// members
#if defined(ENABLE_FALLOUT_VR) && !defined(ENABLE_FALLOUT_NG) && !defined(ENABLE_FALLOUT_F4)
		// VR's camera renders several views (the constructor takes their count): a frustum per view, with
		// viewFrustum pointing at the current one, after the 0x40 bytes VR adds to NiAVObject. Read from
		// Fallout4VR.exe: the constructor 0x1C2BAD0 (allocations are 0x230), the destructor 0x1C2BDC0,
		// SetViewFrustum 0x1C2BFA0 (writes every view's frustum and *viewFrustum, clamped by minNearPlaneDist /
		// maxFarNearRatio), WorldPtToScreenPt3 0x1C2D860 (worldToCam, port).
		std::uint32_t       pad120[0x10];      // 120
		float               worldToCam[4][4];  // 160
		BSTArray<NiFrustum> viewFrustums;      // 1A0
		BSTArray<NiPoint3>  unk1B8;            // 1B8
		BSTArray<NiPoint3>  unk1D0;            // 1D0
		BSTArray<NiMatrix3> unk1E8;            // 1E8
		NiFrustum*          viewFrustum;       // 200
		std::uint32_t       numViews;          // 208
		float               minNearPlaneDist;  // 20C
		float               maxFarNearRatio;   // 210
		NiRect<float>       port;              // 214
		float               lodAdjust;         // 224
#else
		float         worldToCam[4][4];  // 120
		NiFrustum     viewFrustum;       // 160
		float         minNearPlaneDist;  // 17C
		float         maxFarNearRatio;   // 180
		NiRect<float> port;              // 184
		float         lodAdjust;         // 194
#endif
	};
#if defined(ENABLE_FALLOUT_VR) && !defined(ENABLE_FALLOUT_NG) && !defined(ENABLE_FALLOUT_F4)
	static_assert(offsetof(NiCamera, worldToCam) == 0x160);
	static_assert(offsetof(NiCamera, viewFrustum) == 0x200);
	static_assert(offsetof(NiCamera, port) == 0x214);
	static_assert(sizeof(NiCamera) == 0x230);
#else
	static_assert(sizeof(NiCamera) == 0x1A0);
#endif
}
