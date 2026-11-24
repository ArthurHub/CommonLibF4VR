#pragma once

namespace RE::RadioManager
{
	// The frequency the player's radio is tuned to.
	[[nodiscard]] inline float QCurrentPlayerFreq()
	{
		using func_t = decltype(&QCurrentPlayerFreq);
		static REL::Relocation<func_t> func{ REL::RelocationID(227903, 0) };  // VR: tested
		return func();
	}

	// Whether the player's radio is on.
	[[nodiscard]] inline bool QPlayerRadioEnabled()
	{
		using func_t = decltype(&QPlayerRadioEnabled);
		static REL::Relocation<func_t> func{ REL::RelocationID(811921, 0) };  // VR: tested
		return func();
	}
}
