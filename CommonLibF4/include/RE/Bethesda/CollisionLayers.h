#pragma once

#include "RE/Bethesda/bhkCharacterController.h"

// COL_LAYER in human terms: a layer's name, a layer from a name, and layer bitmasks. Every Havok collision-filter
// word carries its layer in the low 7 bits (the rest is the collision group). A raycast that stops in empty air
// reads plainly once its hit layer reads "actorZone" (an invisible AI trigger volume, which collides with
// character controllers and so with any ray carrying the player's filter) rather than "static".
namespace RE::CollisionLayers
{
	namespace detail
	{
		// COL_LAYER by index, its enumerator spelling minus the "k".
		inline constexpr std::string_view NAMES[] = {
			"unidentified", "static", "animStatic", "transparent", "clutter", "weapon", "projectile", "spell",
			"biped", "trees", "props", "water", "trigger", "terrain", "trap", "nonCollidable", "cloudTrap", "ground",
			"portal", "debrisSmall", "debrisLarge", "acousticSpace", "actorZone", "projectileZone", "gasTrap",
			"shellCasing", "transparentSmall", "invisibleWall", "transparentSmallAnim", "clutterLarge",
			"charController", "stairHelper", "deadBip", "bipedNoCC", "avoidBox", "collisionBox", "cameraSphere",
			"doorDetection", "coneProjectile", "cameraPick", "itemPick", "lineOfSight", "pathPick", "unused0",
			"unused1", "spellExplosion", "droppingPick"
		};

		inline constexpr std::uint32_t LAYER_MASK = 0x7F;
	}

	// The layer in a collision-filter word (its low 7 bits).
	[[nodiscard]] constexpr COL_LAYER GetLayer(std::uint32_t a_filter)
	{
		return static_cast<COL_LAYER>(a_filter & detail::LAYER_MASK);
	}

	// The layer's name ("static", "actorZone", ...), or "layer<N>" for a value the enum doesn't cover. Takes a
	// filter word or a bare layer value.
	[[nodiscard]] inline std::string GetName(std::uint32_t a_filter)
	{
		const auto layer = a_filter & detail::LAYER_MASK;
		return layer < std::size(detail::NAMES) ? std::string(detail::NAMES[layer]) : std::format("layer{}", layer);
	}

	// The layer a token names: a layer name as GetName spells it (any case), or a layer number.
	[[nodiscard]] inline std::optional<COL_LAYER> Parse(std::string_view a_token)
	{
		for (std::uint32_t i = 0; i < std::size(detail::NAMES); ++i) {
			const auto name = detail::NAMES[i];
			if (name.size() == a_token.size() && _strnicmp(name.data(), a_token.data(), name.size()) == 0) {
				return static_cast<COL_LAYER>(i);
			}
		}
		std::uint32_t layer = 0;
		const auto    base = a_token.starts_with("0x") || a_token.starts_with("0X") ? 16 : 10;
		const auto    begin = a_token.data() + (base == 16 ? 2 : 0);
		const auto [end, error] = std::from_chars(begin, a_token.data() + a_token.size(), layer, base);
		if (error != std::errc{} || end != a_token.data() + a_token.size() || layer >= std::size(detail::NAMES)) {
			return std::nullopt;
		}
		return static_cast<COL_LAYER>(layer);
	}

	// The bit of a layer in a layer bitmask; nothing for a value past 63.
	[[nodiscard]] constexpr std::uint64_t LayerBit(COL_LAYER a_layer)
	{
		const auto index = static_cast<std::uint32_t>(a_layer);
		return index < 64 ? 1ull << index : 0;
	}

	// The bitmask of a set of layers, at compile time where the set is fixed.
	[[nodiscard]] constexpr std::uint64_t LayerMask(std::initializer_list<COL_LAYER> a_layers)
	{
		std::uint64_t mask = 0;
		for (const auto layer : a_layers) {
			mask |= LayerBit(layer);
		}
		return mask;
	}

	// The bitmask of a comma-separated list of layer names and numbers, as from config. Tokens that name no
	// layer are skipped, and added to a_unknown when it is given.
	[[nodiscard]] inline std::uint64_t ParseMask(std::string_view a_list, std::vector<std::string>* a_unknown = nullptr)
	{
		std::uint64_t mask = 0;
		while (!a_list.empty()) {
			const auto comma = a_list.find(',');
			auto       token = a_list.substr(0, comma);
			a_list = comma == std::string_view::npos ? std::string_view{} : a_list.substr(comma + 1);
			while (!token.empty() && std::isspace(static_cast<unsigned char>(token.front()))) {
				token.remove_prefix(1);
			}
			while (!token.empty() && std::isspace(static_cast<unsigned char>(token.back()))) {
				token.remove_suffix(1);
			}
			if (token.empty()) {
				continue;
			}
			if (const auto layer = Parse(token)) {
				mask |= LayerBit(*layer);
			} else if (a_unknown) {
				a_unknown->emplace_back(token);
			}
		}
		return mask;
	}

	// Whether the layer in a collision-filter word is in a layer bitmask.
	[[nodiscard]] constexpr bool IsInMask(std::uint32_t a_filter, std::uint64_t a_mask)
	{
		const auto layer = a_filter & detail::LAYER_MASK;
		return layer < 64 && (a_mask & (1ull << layer)) != 0;
	}
}
