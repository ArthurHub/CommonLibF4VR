#pragma once

#include "RE/Bethesda/BSTArray.h"
#include "RE/NetImmerse/NiLight.h"
#include "RE/NetImmerse/NiMatrix3.h"
#include "RE/NetImmerse/NiPoint.h"
#include "RE/NetImmerse/NiRefObject.h"
#include "RE/NetImmerse/NiSmartPointer.h"

namespace RE
{
	class BSCullingProcess;
	class BSGeometry;
	class BSLensFlareRenderData;
	class BSMultiBoundNode;
	class BSPortal;
	class BSPortalGraph;
	class NiCamera;
	class NiNode;
	class NiTexture;

	// The renderer's side of a light: the light volume, gobo and culling state the ShadowSceneNode keeps for an
	// NiLight. TESObjectLIGH::GenDynamic builds one per light; BSShadowLight and its subclasses add a shadow map.
	class __declspec(novtable) BSLight :
		public NiRefObject  // 000
	{
	public:
		static constexpr auto RTTI{ RTTI::BSLight };
		static constexpr auto VTABLE{ VTABLE::BSLight };

		// The light volume (geometry) the deferred renderer draws the light with, set by the light factory.
		enum class Shape : std::uint32_t
		{
			kNone = 0,
			kOmni = 2,  // sphere
			kHemi = 3,  // hemisphere, for kHemiShadow lights
			kBox = 5,   // extents in centerAndHalfExtents
			kSpot = 6   // cone, built from spot and the NiLight radius
		};

		struct SpotParams
		{
		public:
			// members
			float falloffExponent;  // 0 - the power of the falloff towards the cone's edge, at least 1
			float cosHalfAngle;     // 4 - cos(FOV / 2), where the cone ends
		};
		static_assert(sizeof(SpotParams) == 0x8);

		virtual ~BSLight();  // 00

		// add
		virtual void               SetLight(NiLight* a_light);  // 02 - also flags point and ambient lights
		[[nodiscard]] virtual bool IsShadowLight() const;       // 03 - BSShadowLight and its subclasses

		// Luminance of the light at the point, also kept in luminance; 0 when the light is a_excluded.
		[[nodiscard]] float GetLuminanceAtPoint(const NiPoint3& a_point, const NiLight* a_excluded)
		{
			using func_t = decltype(&BSLight::GetLuminanceAtPoint);
			static REL::Relocation<func_t> func{ REL::ID(170662) };
			return func(this, a_point, a_excluded);
		}

		// members - layout from F4SE, the same in Fallout4VR.exe: the constructor 0x286D510, SetLight 0x286E870,
		// GetLuminanceAtPoint 0x286EA60, the light factory 0x27E9CD0 (fills a new light from TESObjectLIGH) and
		// the shape setter 0x286F9B0.
		float         lodDimmer;                   // 010
		float         luminance;                   // 014
		std::uint32_t frustumCull;                 // 018
		std::uint32_t pad01C;                      // 01C
		alignas(0x10) float goboProjection[4][4];  // 020
		NiMatrix3 shapeRotation;                   // 060
		NiPoint3A pointPosition;                   // 090 - a dynamic point light's position
		union
		{
			float      centerAndHalfExtents[6];  // Shape::kBox
			SpotParams spot;
		};                                                       // 0A0
		NiPointer<NiLight>                 light;                // 0B8
		void*                              unk0C0;               // 0C0
		void*                              unk0C8;               // 0C8
		std::uint64_t                      unk0D0;               // 0D0
		void*                              unk0D8;               // 0D8
		std::uint64_t                      unk0E0;               // 0E0
		void*                              unk0E8;               // 0E8
		BSTArray<BSMultiBoundNode*>        multiBoundRooms;      // 0F0
		BSTArray<BSPortal*>                portals;              // 108
		BSTArray<NiNode*>                  processedNodes;       // 120
		BSPortalGraph*                     portalGraph;          // 138
		BSCullingProcess*                  cullingProcess;       // 140
		NiPointer<BSGeometry>              geometry;             // 148 - the light volume, see Shape
		NiPointer<NiCamera>                camera;               // 150 - projects the gobo of a light without shadows
		NiPointer<NiTexture>               goboTexture;          // 158
		std::uint32_t                      addFadeNodeLock;      // 160
		std::uint32_t                      lensFlareLastFrame;   // 164
		std::uint32_t                      lensFlareFrameCount;  // 168
		std::uint32_t                      lensFlareIndex;       // 16C
		bool                               pointLight;           // 170
		bool                               ambientLight;         // 171
		bool                               dynamicLight;         // 172
		bool                               portalStrict;         // 173
		bool                               showDebugTexture;     // 174
		bool                               affectLand;           // 175
		bool                               affectWater;          // 176
		bool                               lodFade;              // 177
		bool                               specular;             // 178
		bool                               attenuationOnly;      // 179
		bool                               ignoreRoughness;      // 17A
		bool                               ignoreRim;            // 17B
		bool                               occluded;             // 17C
		bool                               stencilVolume;        // 17D
		bool                               temporary;            // 17E
		std::uint8_t                       pad17F;               // 17F
		REX::EnumSet<Shape, std::uint32_t> shape;                // 180
		std::uint32_t                      pad184;               // 184
		BSLensFlareRenderData*             lensFlare;            // 188
	};
	static_assert(offsetof(BSLight, spot) == 0xA0);
	static_assert(offsetof(BSLight, light) == 0xB8);
	static_assert(offsetof(BSLight, geometry) == 0x148);
	static_assert(offsetof(BSLight, goboTexture) == 0x158);
	static_assert(offsetof(BSLight, shape) == 0x180);
	static_assert(sizeof(BSLight) == 0x190);
}
