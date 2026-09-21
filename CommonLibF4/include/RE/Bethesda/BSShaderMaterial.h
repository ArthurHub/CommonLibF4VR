#pragma once

#include "RE/Bethesda/BSFixedString.h"
#include "RE/Bethesda/BSGraphics.h"
#include "RE/Bethesda/BSLock.h"
#include "RE/Bethesda/BSTSmartPointer.h"
#include "RE/Bethesda/BSTextureDB.h"
#include "RE/NetImmerse/NiColor.h"
#include "RE/NetImmerse/NiPoint.h"
#include "RE/NetImmerse/NiSmartPointer.h"

namespace RE
{
	class BSLightingShaderMaterialEnvmap;
	class BSTextureSet;
	class NiTexture;

	class __declspec(novtable) BSShaderMaterial :
		public BSIntrusiveRefCounted  // 00
	{
	public:
		static constexpr auto RTTI{ RTTI::BSShaderMaterial };
		static constexpr auto VTABLE{ VTABLE::BSShaderMaterial };

		enum class Feature
		{
			kNone = static_cast<std::underlying_type_t<Feature>>(-1),
			kDefault = 0,
			kEnvmap,
			kGlowmap,
			kParallax,
			kFace,
			kSkinTint,
			kHairTint,
			kParallaxOcc,
			kLandscape,
			kLODLandscape,
			kSnow,
			kMultiLayerParallax,
			kTreeAnim,
			kLODObjects,
			kMultiIndexSnow,
			kLODObjectsHD,
			kEye,
			kCloud,
			kLODLandscapeNoise,
			kLODLandscapeBlend,
			kDismemberment
		};

		enum class Type
		{
			kBase = 0,
			kEffect = 1,
			kLighting = 2,
			kWater = 3
		};

		virtual ~BSShaderMaterial();

		// add
		virtual BSShaderMaterial* Create();                                                     // 01
		virtual void              CopyMembers(const BSShaderMaterial* a_other);                 // 02
		virtual std::uint32_t     ComputeCRC32(std::uint32_t a_uniqueID, bool a_combining);     // 03
		virtual BSShaderMaterial* GetDefault();                                                 // 04
		virtual Feature           GetFeature();                                                 // 05
		virtual Type              GetType();                                                    // 06
		virtual void              ReceiveValuesFromRootMaterial(BSShaderProperty* a_property);  // 07
		virtual bool              DoIsCopy(const BSShaderMaterial* a_other);                    // 08

		// hashKey of a material outside the shared material cache (acquired with SetMaterial(material, true))
		static constexpr std::uint32_t UNIQUE_HASH_KEY{ 0xFFFFFFFF };

		// members
#if defined(ENABLE_FALLOUT_VR) && !defined(ENABLE_FALLOUT_NG) && !defined(ENABLE_FALLOUT_F4)
		// VR has one UV offset/scale pair, not two, so every member from hashKey on (and every derived material
		// member) sits 0x10 lower than on flat. Read from Fallout4VR.exe: CopyMembers 0x282E2B0 copies 0x0C-0x1B,
		// BSShaderProperty::SetMaterial 0x278EF20 writes hashKey / uniqueCode.
		NiPoint2      texCoordOffset[1];  // 0C
		NiPoint2      texCoordScale[1];   // 14
		std::uint32_t hashKey;            // 1C
		std::uint32_t uniqueCode;         // 20
#else
		NiPoint2      texCoordOffset[2];  // 0C
		NiPoint2      texCoordScale[2];   // 1C
		std::uint32_t hashKey;            // 2C
		std::uint32_t uniqueCode;         // 30
#endif
	};
#if defined(ENABLE_FALLOUT_VR) && !defined(ENABLE_FALLOUT_NG) && !defined(ENABLE_FALLOUT_F4)
	static_assert(sizeof(BSShaderMaterial) == 0x28);
#else
	static_assert(sizeof(BSShaderMaterial) == 0x38);
#endif

	class __declspec(novtable) BSEffectShaderMaterial :
		public BSShaderMaterial  // 00
	{
	public:
		static constexpr auto RTTI{ RTTI::BSEffectShaderMaterial };
		static constexpr auto VTABLE{ VTABLE::BSEffectShaderMaterial };

		// members - offsets are VR's, read from Fallout4VR.exe: Create 0x2894720 allocates 0xB0, CopyMembers
		// 0x2894780 and BSEffectShaderProperty::LoadBinary 0x279DFC0 place the members, the draw setup 0x279E950
		// reads baseTexture. Flat is expected 0x10 higher (the larger base) but unverified.
		float                falloffStartAngle;      // 28 - cosine of the view angle the falloff starts at
		float                falloffStopAngle;       // 2C
		float                falloffStartOpacity;    // 30
		float                falloffStopOpacity;     // 34
		NiColorA             baseColor;              // 38 - alpha is the material alpha
		NiPointer<NiTexture> baseTexture;            // 48
		NiPointer<NiTexture> grayscaleTexture;       // 50
		NiPointer<NiTexture> envMapTexture;          // 58
		NiPointer<NiTexture> envMapMaskTexture;      // 60
		NiPointer<NiTexture> normalTexture;          // 68
		float                softDepth;              // 70
		float                baseColorScale;         // 74 - emissive multiplier
		BSFixedString        baseTextureName;        // 78 - Data-relative, "Textures\..."
		BSFixedString        grayscaleTextureName;   // 80
		BSFixedString        envMapTextureName;      // 88
		BSFixedString        envMapMaskTextureName;  // 90
		BSFixedString        normalTextureName;      // 98
		float                envMapScale;            // A0
		std::uint8_t         textureClampMode;       // A4
		std::uint8_t         lightingInfluence;      // A5
		std::uint8_t         envMapMinLOD;           // A6
		std::uint8_t         padA7;                  // A7
		std::uint32_t        unkA8;                  // A8
	};
#if defined(ENABLE_FALLOUT_VR) && !defined(ENABLE_FALLOUT_NG) && !defined(ENABLE_FALLOUT_F4)
	static_assert(sizeof(BSEffectShaderMaterial) == 0xB0);
#else
	static_assert(sizeof(BSEffectShaderMaterial) == 0xC0);
#endif

	struct __declspec(novtable) BSLightingShaderMaterialBase :
		public BSShaderMaterial  // 00
	{
	public:
		static constexpr auto RTTI{ RTTI::BSLightingShaderMaterialBase };
		static constexpr auto VTABLE{ VTABLE::BSLightingShaderMaterialBase };

		// add
		virtual const BSLightingShaderMaterialEnvmap* IsLightingShaderMaterialEnvmap();                                                     // 09
		virtual void                                  ClearTextures();                                                                      // 0A
		virtual std::uint32_t                         GetTextures(NiTexture** a_textures, std::uint32_t a_arraySize);                       // 0B
		virtual void                                  SaveBinary(NiStream* a_stream);                                                       // 0C
		virtual void                                  LoadBinary(NiStream* a_stream);                                                       // 0D
		virtual void                                  OnPrefetchTextures(TextureDBHandle* a_outHandles, const BSTextureSet* a_textureSet);  // 0E
		virtual void                                  OnLoadTextureSet1(const BSTextureSet* a_textureSet, TextureDBHandle* a_inHandles);    // 0F
		virtual void                                  OnLoadTextureSet2(const BSTextureSet* a_textureSet);                                  // 10
		virtual void                                  DoReceiveValuesFromRootMaterial(const BSShaderData& a_rootData);                      // 11

		void OnLoadTextureSet(const BSTextureSet* a_textureSet, TextureDBHandle* a_inHandles)
		{
			return OnLoadTextureSet1(a_textureSet, a_inHandles);
		}

		void OnLoadTextureSet(const BSTextureSet* a_textureSet)
		{
			return OnLoadTextureSet2(a_textureSet);
		}

		// members
		NiColor                        specularColor;                  // 38
		NiPointer<NiTexture>           diffuseTexture;                 // 40
		NiPointer<NiTexture>           normalTexture;                  // 48
		NiPointer<NiTexture>           rimSoftLightingTexture;         // 50
		NiPointer<NiTexture>           smoothnessSpecMaskTexture;      // 58
		NiPointer<NiTexture>           lookupTexture;                  // 60
		BSGraphics::TextureAddressMode textureClampMode;               // 68
		NiPointer<BSTextureSet>        textureSet;                     // 70
		float                          materialAlpha;                  // 74
		float                          refractionPower;                // 78
		float                          smoothness;                     // 7C
		float                          specularColorScale;             // 80
		float                          fresnelPower;                   // 84
		float                          wetnessControl_SpecScale;       // 88
		float                          wetnessControl_SpecPowerScale;  // 8C
		float                          wetnessControl_SpecMin;         // 90
		float                          wetnessControl_EnvMapScale;     // 94
		float                          wetnessControl_FresnelPower;    // 98
		float                          wetnessControl_Metalness;       // 9C
		float                          subSurfaceLightRolloff;         // A0
		float                          rimLightPower;                  // A4
		float                          backLightPower;                 // A8
		float                          lookupScale;                    // AC
		BSNonReentrantSpinLock         loadTextureSetLock;             // B0
	};
	// VR: the same members 0x10 lower, after the smaller base (Fallout4VR.exe: Create 0x280D140 allocates 0xB0,
	// CopyMembers 0x280BB10 copies specularColor at 0x28 through lookupScale at 0xA8)
#if defined(ENABLE_FALLOUT_VR) && !defined(ENABLE_FALLOUT_NG) && !defined(ENABLE_FALLOUT_F4)
	static_assert(sizeof(BSLightingShaderMaterialBase) == 0xB0);
#else
	static_assert(sizeof(BSLightingShaderMaterialBase) == 0xC0);
#endif

	struct __declspec(novtable) BSLightingShaderMaterial :
		public BSLightingShaderMaterialBase  // 00
	{
	public:
		static constexpr auto RTTI{ RTTI::BSLightingShaderMaterial };
		static constexpr auto VTABLE{ VTABLE::BSLightingShaderMaterial };
	};
#if defined(ENABLE_FALLOUT_VR) && !defined(ENABLE_FALLOUT_NG) && !defined(ENABLE_FALLOUT_F4)
	static_assert(sizeof(BSLightingShaderMaterial) == 0xB0);
#else
	static_assert(sizeof(BSLightingShaderMaterial) == 0xC0);
#endif
}
