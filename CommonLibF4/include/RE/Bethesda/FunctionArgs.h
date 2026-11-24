/*
MIT License

Copyright (c) 2022 shad0wshayd3

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include "RE/Bethesda/BSScriptUtil.h"

namespace Papyrus
{
	template <class F>
	using BSTThreadScrapFunctionOG = RE::msvc::function<F>;  // Used by OG/VR for DispatchMethod calls

#if defined(ENABLE_FALLOUT_F4) || defined(ENABLE_FALLOUT_VR)
	/**
	 * The arguments of a Papyrus call on OG and VR, as the game's own arguments callable. The variadic
	 * IVirtualMachine::DispatchStaticCall / DispatchMethodCall build these themselves; this is for calling the virtuals directly.
	 */
	template <class... Args>
	class FunctionArgs : public RE::BSScript::detail::ScrapFunctionArguments
	{
	public:
		FunctionArgs(RE::BSScript::IVirtualMachine* a_vm, Args... a_args) :
			ScrapFunctionArguments(*a_vm, a_args...)
		{}

		BSTThreadScrapFunctionOG<bool(RE::BSScrapArray<RE::BSScript::Variable>&)> get() { return Get(); }
	};
#endif
}
