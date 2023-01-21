#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "dvars.hpp"

#include "game/s1/game.hpp"

#include <utils/hook.hpp>

namespace s1
{
	namespace dvars
	{
		struct dvar_base
		{
			unsigned int flags{};
		};

		struct dvar_bool : dvar_base
		{
			bool value{};
		};

		struct dvar_float : dvar_base
		{
			float value{};
			float min{};
			float max{};
		};

		struct dvar_vector2 : dvar_base
		{
			float x{};
			float y{};
			float min{};
			float max{};
		};

		struct dvar_vector3 : dvar_base
		{

			float x{};
			float y{};
			float z{};
			float min{};
			float max{};
		};

		struct dvar_int : dvar_base
		{
			int value{};
			int min{};
			int max{};
		};

		struct dvar_string : dvar_base
		{
			std::string value{};
		};

		namespace
		{
			template <typename T>
			T* find_dvar(std::unordered_map<std::string, T>& map, const std::string& name)
			{
				auto i = map.find(name);
				if (i != map.end())
				{
					return &i->second;
				}

				return nullptr;
			}

			bool find_dvar(std::unordered_set<std::string>& set, const std::string& name)
			{
				return set.find(name) != set.end();
			}
		}

		namespace disable
		{
			static std::unordered_set<std::string> set_bool_disables;
			static std::unordered_set<std::string> set_float_disables;
			static std::unordered_set<std::string> set_int_disables;
			static std::unordered_set<std::string> set_string_disables;

			void set_bool(const std::string& name)
			{
				set_bool_disables.emplace(name);
			}

			void set_float(const std::string& name)
			{
				set_float_disables.emplace(name);
			}

			void set_int(const std::string& name)
			{
				set_int_disables.emplace(name);
			}

			void set_string(const std::string& name)
			{
				set_string_disables.emplace(name);
			}
		}

		namespace override
		{
			static std::unordered_map<std::string, dvar_bool> register_bool_overrides;
			static std::unordered_map<std::string, dvar_float> register_float_overrides;
			static std::unordered_map<std::string, dvar_int> register_int_overrides;
			static std::unordered_map<std::string, dvar_string> register_string_overrides;
			static std::unordered_map<std::string, dvar_vector2> register_vector2_overrides;
			static std::unordered_map<std::string, dvar_vector3> register_vector3_overrides;

			static std::unordered_map<std::string, bool> set_bool_overrides;
			static std::unordered_map<std::string, float> set_float_overrides;
			static std::unordered_map<std::string, int> set_int_overrides;
			static std::unordered_map<std::string, std::string> set_string_overrides;

			void register_bool(const std::string& name, const bool value, const unsigned int flags)
			{
				dvar_bool values;
				values.value = value;
				values.flags = flags;
				register_bool_overrides[name] = std::move(values);
			}

			void register_float(const std::string& name, const float value, const float min, const float max, const unsigned int flags)
			{
				dvar_float values;
				values.value = value;
				values.min = min;
				values.max = max;
				values.flags = flags;
				register_float_overrides[name] = std::move(values);
			}

			void register_int(const std::string& name, const int value, const int min, const int max, const unsigned int flags)
			{
				dvar_int values;
				values.value = value;
				values.min = min;
				values.max = max;
				values.flags = flags;
				register_int_overrides[name] = std::move(values);
			}

			void register_string(const std::string& name, const std::string& value, const unsigned int flags)
			{
				dvar_string values;
				values.value = value;
				values.flags = flags;
				register_string_overrides[name] = std::move(values);
			}

			void register_vector2(const std::string& name, float x, float y, float min, float max, const unsigned int flags)
			{
				dvar_vector2 values;
				values.x = x;
				values.y = y;
				values.min = min;
				values.max = max;
				values.flags = flags;
				register_vector2_overrides[name] = std::move(values);
			}

			void register_vector3(const std::string& name, float x, float y, float z, float min, float max, const unsigned int flags)
			{
				dvar_vector3 values;
				values.x = x;
				values.y = y;
				values.z = z;
				values.min = min;
				values.max = max;
				values.flags = flags;
				register_vector3_overrides[name] = std::move(values);
			}

			void set_bool(const std::string& name, const bool value)
			{
				set_bool_overrides[name] = value;
			}

			void set_float(const std::string& name, const float value)
			{
				set_float_overrides[name] = value;
			}

			void set_int(const std::string& name, const int value)
			{
				set_int_overrides[name] = value;
			}

			void set_string(const std::string& name, const std::string& value)
			{
				set_string_overrides[name] = value;
			}
		}

		utils::hook::detour dvar_register_bool_hook;
		utils::hook::detour dvar_register_float_hook;
		utils::hook::detour dvar_register_int_hook;
		utils::hook::detour dvar_register_string_hook;
		utils::hook::detour dvar_register_vector2_hook;
		utils::hook::detour dvar_register_vector3_hook;

		utils::hook::detour dvar_set_bool_hook;
		utils::hook::detour dvar_set_float_hook;
		utils::hook::detour dvar_set_int_hook;
		utils::hook::detour dvar_set_string_hook;

		game::dvar_t* dvar_register_bool_stub(const char* name, bool value, unsigned int flags, const char* description)
		{
			auto* var = find_dvar(override::register_bool_overrides, name);
			if (var)
			{
				value = var->value;
				flags = var->flags;
			}

			return dvar_register_bool_hook.invoke<game::dvar_t*>(name, value, flags, description);
		}

		game::dvar_t* dvar_register_float_stub(const char* name, float value, float min, float max, unsigned int flags,
			const char* description)
		{
			auto* var = find_dvar(override::register_float_overrides, name);
			if (var)
			{
				value = var->value;
				min = var->min;
				max = var->max;
				flags = var->flags;
			}

			return dvar_register_float_hook.invoke<game::dvar_t*>(name, value, min, max, flags, description);
		}

		game::dvar_t* dvar_register_int_stub(const char* name, int value, int min, int max, unsigned int flags,
			const char* description)
		{
			auto* var = find_dvar(override::register_int_overrides, name);
			if (var)
			{
				value = var->value;
				min = var->min;
				max = var->max;
				flags = var->flags;
			}

			return dvar_register_int_hook.invoke<game::dvar_t*>(name, value, min, max, flags, description);
		}

		game::dvar_t* dvar_register_string_stub(const char* name, const char* value, unsigned int flags, const char* description)
		{
			auto* var = find_dvar(override::register_string_overrides, name);
			if (var)
			{
				value = var->value.data();
				flags = var->flags;
			}

			return dvar_register_string_hook.invoke<game::dvar_t*>(name, value, flags, description);
		}

		game::dvar_t* dvar_register_vector2_stub(const char* name, float x, float y, float min, float max,
			unsigned int flags, const char* description)
		{
			auto* var = find_dvar(override::register_vector2_overrides, name);
			if (var)
			{
				x = var->x;
				y = var->y;
				min = var->min;
				max = var->max;
				flags = var->flags;
			}

			return dvar_register_vector2_hook.invoke<game::dvar_t*>(name, x, y, min, max, flags, description);
		}

		game::dvar_t* dvar_register_vector3_stub(const char* name, float x, float y, float z, float min,
			float max, unsigned int flags, const char* description)
		{
			auto* var = find_dvar(override::register_vector3_overrides, name);
			if (var)
			{
				x = var->x;
				y = var->y;
				z = var->z;
				min = var->min;
				max = var->max;
				flags = var->flags;
			}

			return dvar_register_vector3_hook.invoke<game::dvar_t*>(name, x, y, z, min, max, flags, description);
		}

		void dvar_set_bool_stub(game::dvar_t* dvar, bool boolean)
		{
			const auto disabled = find_dvar(disable::set_bool_disables, dvar->name);
			if (disabled)
			{
				return;
			}

			auto* var = find_dvar(override::set_bool_overrides, dvar->name);
			if (var)
			{
				boolean = *var;
			}

			return dvar_set_bool_hook.invoke<void>(dvar, boolean);
		}

		void dvar_set_float_stub(game::dvar_t* dvar, float fl)
		{
			const auto disabled = find_dvar(disable::set_float_disables, dvar->name);
			if (disabled)
			{
				return;
			}

			auto* var = find_dvar(override::set_float_overrides, dvar->name);
			if (var)
			{
				fl = *var;
			}

			return dvar_set_float_hook.invoke<void>(dvar, fl);
		}

		void dvar_set_int_stub(game::dvar_t* dvar, int integer)
		{
			const auto disabled = find_dvar(disable::set_int_disables, dvar->name);
			if (disabled)
			{
				return;
			}

			auto* var = find_dvar(override::set_int_overrides, dvar->name);
			if (var)
			{
				integer = *var;
			}

			return dvar_set_int_hook.invoke<void>(dvar, integer);
		}

		void dvar_set_string_stub(game::dvar_t* dvar, const char* string)
		{
			const auto disabled = find_dvar(disable::set_string_disables, dvar->name);
			if (disabled)
			{
				return;
			}

			auto* var = find_dvar(override::set_string_overrides, dvar->name);
			if (var)
			{
				string = var->data();
			}

			return dvar_set_string_hook.invoke<void>(dvar, string);
		}

		class component final : public component_interface
		{
		public:
			void post_unpack() override
			{
				dvar_register_bool_hook.create(0x1404C0BE0, &dvar_register_bool_stub);
				dvar_register_float_hook.create(0x1404C0FB0, &dvar_register_float_stub);
				dvar_register_int_hook.create(0x1404C1080, &dvar_register_int_stub);
				dvar_register_string_hook.create(0x1404C1450, &dvar_register_string_stub);
				dvar_register_vector2_hook.create(0x1404C1520, &dvar_register_vector2_stub);
				dvar_register_vector3_hook.create(0x1404C1600, &dvar_register_vector3_stub);

				dvar_set_bool_hook.create(0x1404C1F30, &dvar_set_bool_stub);
				dvar_set_float_hook.create(0x1404C2A10, &dvar_set_float_stub);
				dvar_set_int_hook.create(0x1404C2F40, &dvar_set_int_stub);
				dvar_set_string_hook.create(0x1404C3610, &dvar_set_string_stub);
			}
		};
	}
}

REGISTER_COMPONENT_S1(s1::dvars::component)
