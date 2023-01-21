#pragma once
#include "component_interface.hpp"

#include "game/mode.hpp"

class component_loader final
{
public:
	class premature_shutdown_trigger final : public std::exception
	{
		[[nodiscard]] const char* what() const noexcept override
		{
			return "Premature shutdown requested";
		}
	};

	template <typename T>
	class installer final
	{
		static_assert(std::is_base_of<component_interface, T>::value, "component has invalid base class");

	public:
		installer()
		{
			register_component(std::make_unique<T>());
		}
		installer(game::game_mode mode, const std::string& component_name)
		{
			register_component(std::make_unique<T>(), mode, component_name);
		}
	};

	template <typename T>
	static T* get()
	{
		for (const auto& component_ : get_components())
		{
			if (typeid(*component_.get()) == typeid(T))
			{
				return reinterpret_cast<T*>(component_.get());
			}
		}

		return nullptr;
	}

	static void register_component(std::unique_ptr<component_interface>&& component, 
		game::game_mode target_mode_ = game::game_mode::none, const std::string& component_name_ = "void");

	static bool post_start();
	static bool post_load();
	static void post_unpack();
	static void pre_destroy();
	static void clean();

	static void* load_import(const std::string& library, const std::string& function);

	static void trigger_premature_shutdown();

private:
	static std::vector<std::unique_ptr<component_interface>>& get_components();
};

#define REGISTER_COMPONENT(name) \
namespace \
{ \
	static component_loader::installer<name> __component = component_loader::installer<name>(game::game_mode::none, #name); \
}

#define REGISTER_COMPONENT_H1(name) \
namespace \
{ \
	static component_loader::installer<name> __component = component_loader::installer<name>(game::game_mode::h1, #name); \
}

#define REGISTER_COMPONENT_H2(name) \
namespace \
{ \
	static component_loader::installer<name> __component = component_loader::installer<name>(game::game_mode::h2, #name); \
}

#define REGISTER_COMPONENT_S1(name) \
namespace \
{ \
	static component_loader::installer<name> __component = component_loader::installer<name>(game::game_mode::s1, #name); \
}