#pragma once

#include "RE/NetImmerse/NiAVObject.h"
#include "RE/NetImmerse/NiNode.h"

// Scene graph helpers over NiAVObject / NiNode: finding objects by name, visibility, and bringing world
// transforms up to date after local changes. Names compare case-insensitively, as the engine's do. A max depth
// counts levels below the start: 0 looks at the start only, 1 at it and its children.
namespace RE::NiNodeUtils
{
	// The first child of the object, if it is a node with children.
	[[nodiscard]] inline NiAVObject* GetFirstChild(NiAVObject* a_object)
	{
		const auto node = a_object ? a_object->IsNode() : nullptr;
		return node && !node->children.empty() ? node->children[0].get() : nullptr;
	}

	// The first object named a_name at or under a_root, depth first.
	[[nodiscard]] inline NiAVObject* FindObject(NiAVObject* a_root, std::string_view a_name, int a_maxDepth = INT_MAX)
	{
		if (!a_root || a_maxDepth < 0) {
			return nullptr;
		}
		const std::string_view name{ a_root->name.c_str() };
		if (name.size() == a_name.size() && _strnicmp(name.data(), a_name.data(), a_name.size()) == 0) {
			return a_root;
		}
		if (const auto node = a_root->IsNode()) {
			for (const auto& child : node->children) {
				if (const auto found = FindObject(child.get(), a_name, a_maxDepth - 1)) {
					return found;
				}
			}
		}
		return nullptr;
	}

	// The first object whose name starts with a_prefix at or under a_root, depth first.
	[[nodiscard]] inline NiAVObject* FindObjectStartsWith(NiAVObject* a_root, std::string_view a_prefix, int a_maxDepth = INT_MAX)
	{
		if (!a_root || a_maxDepth < 0) {
			return nullptr;
		}
		const std::string_view name{ a_root->name.c_str() };
		if (name.size() >= a_prefix.size() && _strnicmp(name.data(), a_prefix.data(), a_prefix.size()) == 0) {
			return a_root;
		}
		if (const auto node = a_root->IsNode()) {
			for (const auto& child : node->children) {
				if (const auto found = FindObjectStartsWith(child.get(), a_prefix, a_maxDepth - 1)) {
					return found;
				}
			}
		}
		return nullptr;
	}

	// The first node named a_name at or under a_root, searching nodes only; null when the first object of that
	// name is not a node.
	[[nodiscard]] inline NiNode* FindNode(NiAVObject* a_root, std::string_view a_name, int a_maxDepth = INT_MAX)
	{
		if (!a_root || a_maxDepth < 0) {
			return nullptr;
		}
		const std::string_view name{ a_root->name.c_str() };
		if (name.size() == a_name.size() && _strnicmp(name.data(), a_name.data(), a_name.size()) == 0) {
			return a_root->IsNode();
		}
		if (const auto node = a_root->IsNode()) {
			for (const auto& child : node->children) {
				const auto childNode = child ? child->IsNode() : nullptr;
				if (const auto found = childNode ? FindNode(childNode, a_name, a_maxDepth - 1) : nullptr) {
					return found;
				}
			}
		}
		return nullptr;
	}

	// The first node whose name starts with a_prefix at or under a_root.
	[[nodiscard]] inline NiNode* FindNodeStartsWith(NiAVObject* a_root, std::string_view a_prefix, int a_maxDepth = INT_MAX)
	{
		const auto found = FindObjectStartsWith(a_root, a_prefix, a_maxDepth);
		return found ? found->IsNode() : nullptr;
	}

	// Visible: not null and the hidden flag (bit 0 of flags) clear.
	[[nodiscard]] inline bool IsVisible(const NiAVObject* a_object)
	{
		return a_object && (a_object->flags.flags & 0x1) == 0;
	}

	inline void SetVisible(NiAVObject* a_object, bool a_visible)
	{
		if (a_object) {
			a_object->flags.flags = a_visible ? (a_object->flags.flags & ~0x1ull) : (a_object->flags.flags | 0x1ull);
		}
	}

	// SetVisible on everything under a_root, and a_root itself when a_includeRoot.
	inline void SetVisibleDeep(NiAVObject* a_root, bool a_visible, bool a_includeRoot = true)
	{
		if (!a_root) {
			return;
		}
		if (a_includeRoot) {
			SetVisible(a_root, a_visible);
		}
		if (const auto node = a_root->IsNode()) {
			for (const auto& child : node->children) {
				SetVisibleDeep(child.get(), a_visible, true);
			}
		}
	}

	// UpdateWorldData on everything under a_root, and on a_root when a_includeRoot, skipping the child subtree
	// named a_skipName.
	inline void UpdateWorldDataDown(NiAVObject* a_root, bool a_includeRoot = true, std::string_view a_skipName = {})
	{
		if (!a_root) {
			return;
		}
		NiUpdateData* data = nullptr;
		if (a_includeRoot) {
			a_root->UpdateWorldData(data);
		}
		if (const auto node = a_root->IsNode()) {
			for (const auto& child : node->children) {
				if (!child || (!a_skipName.empty() && _stricmp(child->name.c_str(), std::string(a_skipName).c_str()) == 0)) {
					continue;
				}
				if (const auto childNode = child->IsNode()) {
					UpdateWorldDataDown(childNode, true);
				} else if (const auto geometry = child->IsGeometry()) {
					geometry->UpdateWorldData(data);
				}
			}
		}
	}

	// UpdateWorldData from a_from down through its nodes until the one named as a_to (a_from too when
	// a_includeFrom).
	inline void UpdateWorldDataDownTo(NiNode* a_to, NiNode* a_from, bool a_includeFrom = true)
	{
		if (!a_to || !a_from) {
			return;
		}
		if (a_includeFrom) {
			NiUpdateData* data = nullptr;
			a_from->UpdateWorldData(data);
		}
		if (_stricmp(a_to->name.c_str(), a_from->name.c_str()) == 0) {
			return;
		}
		for (const auto& child : a_from->children) {
			if (const auto childNode = child ? child->IsNode() : nullptr) {
				UpdateWorldDataDownTo(a_to, childNode, true);
			}
		}
	}

	// UpdateWorldData from a_from up through its parents to the one named as a_to (a_to itself when
	// a_includeTo).
	inline void UpdateWorldDataUpTo(NiNode* a_to, NiNode* a_from, bool a_includeTo = true)
	{
		if (!a_to || !a_from) {
			return;
		}
		NiUpdateData* data = nullptr;
		if (_stricmp(a_to->name.c_str(), a_from->name.c_str()) == 0) {
			if (a_includeTo) {
				a_from->UpdateWorldData(data);
			}
			return;
		}
		a_from->UpdateWorldData(data);
		if (a_from->parent) {
			UpdateWorldDataUpTo(a_to, a_from->parent, true);
		}
	}

	// The object's world transform from its parent's world transform and its own local one, without the
	// engine's update: a cheap way to see a local change take effect this frame.
	inline void UpdateTransformFromParent(NiAVObject* a_object)
	{
		if (!a_object || !a_object->parent) {
			return;
		}
		const auto& parent = a_object->parent->world;
		const auto& local = a_object->local;
		a_object->world.translate = parent.translate + parent.rotate.Transpose() * (local.translate * parent.scale);
		a_object->world.rotate = local.rotate * parent.rotate;
		a_object->world.scale = parent.scale * local.scale;
	}

	// UpdateTransformFromParent on everything under a_root (nodes and triangle shapes), and a_root when
	// a_includeRoot, skipping the child subtree named a_skipName. Reaches nodes the engine's update leaves.
	inline void UpdateTransformsDown(NiAVObject* a_root, bool a_includeRoot = true, std::string_view a_skipName = {})
	{
		if (!a_root) {
			return;
		}
		if (a_includeRoot) {
			UpdateTransformFromParent(a_root);
		}
		if (const auto node = a_root->IsNode()) {
			for (const auto& child : node->children) {
				if (!child || (!a_skipName.empty() && _stricmp(child->name.c_str(), std::string(a_skipName).c_str()) == 0)) {
					continue;
				}
				if (const auto childNode = child->IsNode()) {
					UpdateTransformsDown(childNode, true);
				} else if (const auto shape = child->IsTriShape()) {
					UpdateTransformFromParent(shape);
				}
			}
		}
	}
}
