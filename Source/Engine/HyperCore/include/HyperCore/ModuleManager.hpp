/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperCore/Prerequisites.hpp"

#include "HyperCore/Hasher.hpp"
#include "HyperCore/Logger.hpp"
#include "HyperCore/IModule.hpp"
#include "HyperCore/ModuleId.hpp"

#include <string>
#include <type_traits>
#include <vector>

namespace HyperEngine
{
	class ModuleManager
	{
	public:
		HYPERENGINE_MAKE_SINGLETON(ModuleManager);
		
	private:
		struct Module
		{
			uint32_t module_id;
			IModule* module;
		};

	public:
		static auto initialize() -> bool;
		static auto terminate() -> void;
		
		// TODO(SkillerRaptor): Refactoring register_module()
		template <class T, typename... Args>
		static auto register_module(Args&&... args) -> bool
		{
			static_assert(std::is_base_of<IModule, T>::value, "'T' is not a base of IModule!");
			
			T* module = new T(std::forward<Args>(args)...);
			constexpr uint32_t module_id = ModuleId<T>::id();
			
			module->initialize();
			
			s_modules.push_back({module_id, module});

			Logger::debug("Registered {} module", module->name());

			return true;
		}
		
		// TODO(SkillerRaptor): Refactoring unregister_module()
		template <class T>
		static auto unregister_module() -> bool
		{
			static_assert(std::is_base_of<IModule, T>::value, "'T' is not a base of IModule!");
			
			constexpr uint32_t module_id = ModuleId<T>::id();
			
			T* module = nullptr;
			for (Module& current_module : s_modules)
			{
				if(current_module.module_id == module_id)
				{
					module = &current_module;
				}
			}
			module->terminate();
			
			Logger::debug("Unregistered {} module", module->name());
			
			delete module;

			return true;
		}

		template <class T>
		static auto get_module() -> T&
		{
			constexpr uint32_t module_id = ModuleId<T>::id();
			T* module = nullptr;
			for (Module& current_module : s_modules)
			{
				if(current_module.module_id == module_id)
				{
					module = &current_module;
				}
			}
			
			return *module;
		}
		
	private:
		static std::vector<Module> s_modules;
	};
} // namespace HyperEngine
