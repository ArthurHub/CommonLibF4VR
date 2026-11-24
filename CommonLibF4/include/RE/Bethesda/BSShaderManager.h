#pragma once

#include "RE/NetImmerse/NiSmartPointer.h"

namespace RE
{
	class NiTexture;

	class BSShaderManager
	{
	public:
		// The texture at a path such as "Data\Textures\Effects\Gobos\FlashlightGobo01.DDS", loaded or taken from
		// the game's texture cache, which is keyed by the path: a texture loaded here is the one the game finds
		// later under that path. Called as (path, true, texture, false, false, false).
		static void GetTexture(const char* a_path, bool a_demand, NiPointer<NiTexture>& a_texture, bool a_unk1, bool a_unk2, bool a_unk3)
		{
			using func_t = decltype(&BSShaderManager::GetTexture);
			static REL::Relocation<func_t> func{ REL::RelocationID(1375091, 0) };  // VR: tested
			return func(a_path, a_demand, a_texture, a_unk1, a_unk2, a_unk3);
		}
	};
}
