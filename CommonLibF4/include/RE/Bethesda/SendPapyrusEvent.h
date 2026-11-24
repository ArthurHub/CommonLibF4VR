#pragma once
#include <F4SE/API.h>
#include <F4SE/interfaces.h>
#include <RE/Bethesda/BSScriptUtil.h>

/*
Functions for calling Papyrus functions from C++, on every runtime: the variadic
IVirtualMachine::DispatchStaticCall / DispatchMethodCall pass the arguments in the
callable layout each runtime expects.

Usage: in Papyrus, use 'RegisterForExternalEvent'
in C++, call 'SendPapyrusExternalEvent'
*/

namespace Papyrus
{
	namespace detail
	{
		// Generic DispatchStaticCall used to call global Papyrus functions

		template <class... Args>
		bool DispatchStaticCall(
			RE::BSScript::IVirtualMachine*                                  vm,          // VM pointer
			const RE::BSFixedString&                                        a_objName,   // Papyrus script name
			const RE::BSFixedString&                                        a_funcName,  // Papyrus function name
			const RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>& a_callback,
			Args... a_args)  // Args, must match Papyrus declaration
		{
			return vm->DispatchStaticCall(a_objName, a_funcName, a_callback, a_args...);
		}

		// Generic DispatchMethodCall used to call Papyrus function on a specific script instance

		template <class... Args>
		bool DispatchMethodCall(
			RE::BSScript::IVirtualMachine*                                  vm,           // VM pointer
			std::uint64_t                                                   a_objHandle,  // Handle to script instance
			const RE::BSFixedString&                                        a_objName,    // Papyrus Script name
			const RE::BSFixedString&                                        a_funcName,   // Papyrus function name
			const RE::BSTSmartPointer<RE::BSScript::IStackCallbackFunctor>& a_callback,
			Args... a_args)  // Args, must match Papyrus declaration
		{
			return vm->DispatchMethodCall(a_objHandle, a_objName, a_funcName, a_callback, a_args...);
		}

		// Call all the Papyrus functions registered with a_eventName
		// Call RegisterForExternalEvent to register Papyrus functions to be called.

		template <class... Args>
		void SendPapyrusExternalEvent(std::string a_eventName, Args... a_args)
		{
			struct PapyrusEventData
			{
				RE::BSScript::IVirtualMachine* vm;
				std::tuple<Args...>            args;
			};

			auto const       papyrus = F4SE::GetPapyrusInterface();
			PapyrusEventData eventData{ RE::GameVM::GetSingleton()->GetVM().get(), { a_args... } };

			papyrus->GetExternalEventRegistrations(
				a_eventName, &eventData,
				[](uint64_t handle, const char* scriptName, const char* callbackName, void* dataPtr) {
					PapyrusEventData* d = static_cast<PapyrusEventData*>(dataPtr);
					std::apply(
						[&](auto&... a_eventArgs) {
							d->vm->DispatchMethodCall(handle, scriptName, callbackName, nullptr, a_eventArgs...);
						},
						d->args);
				});
		}
	}
}
