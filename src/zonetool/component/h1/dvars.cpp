#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "dvars.hpp"

#include "game/h1/game.hpp"

#include <utils/hook.hpp>

namespace h1
{
	namespace dvars
	{
		struct dvar_combined
		{
			std::string string{};
			game::dvar_value value{};
			game::dvar_limits limits{};
			unsigned int flags{};
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

			template <typename T>
			T* find_dvar(std::unordered_map<std::string, T>& map, const int hash)
			{
				for (auto i = map.begin(); i != map.end(); ++i)
				{
					if (game::generateHashValue(i->first.data()) == hash)
					{
						return &i->second;
					}
				}

				return nullptr;
			}

			bool find_dvar(std::unordered_set<std::string>& set, const std::string& name)
			{
				return set.find(name) != set.end();
			}


			bool find_dvar(std::unordered_set<std::string>& set, const int hash)
			{
				for (auto i = set.begin(); i != set.end(); ++i)
				{
					if (game::generateHashValue(i->data()) == hash)
					{
						return true;
					}
				}

				return false;
			}
		}

		namespace override
		{
			static std::unordered_map<uint32_t, std::unordered_map<std::string, dvar_combined>> register_overrides;
			static std::unordered_map<uint32_t, std::unordered_map<std::string, dvar_combined>> set_overrides;

			void register_bool(const std::string& name, const bool value, const unsigned int flags)
			{
				dvar_combined values;
				values.value.enabled = value;
				values.flags = flags;
				register_overrides[game::dvar_type::boolean][name] = std::move(values);
			}

			void register_float(const std::string& name, const float value, const float min, const float max,
				const unsigned int flags)
			{
				dvar_combined values;
				values.value.value = value;
				values.limits.value.min = min;
				values.limits.value.max = max;
				values.flags = flags;
				register_overrides[game::dvar_type::value][name] = std::move(values);
			}

			void register_int(const std::string& name, const int value, const int min, const int max,
				const unsigned int flags)
			{
				dvar_combined values;
				values.value.integer = value;
				values.limits.integer.min = min;
				values.limits.integer.max = max;
				values.flags = flags;
				register_overrides[game::dvar_type::integer][name] = std::move(values);
			}

			void register_string(const std::string& name, const std::string& value,
				const unsigned int flags)
			{
				dvar_combined values;
				values.string = value;
				values.flags = flags;
				register_overrides[game::dvar_type::string][name] = std::move(values);
			}

			void register_vec2(const std::string& name, float x, float y, float min, float max,
				const unsigned int flags)
			{
				dvar_combined values;
				values.value.vector[0] = x;
				values.value.vector[1] = y;
				values.limits.vector.min = min;
				values.limits.vector.max = max;
				values.flags = flags;
				register_overrides[game::dvar_type::vec2][name] = std::move(values);
			}

			void register_vec3(const std::string& name, float x, float y, float z, float min,
				float max, const unsigned int flags)
			{
				dvar_combined values;
				values.value.vector[0] = x;
				values.value.vector[1] = y;
				values.value.vector[2] = z;
				values.limits.vector.min = min;
				values.limits.vector.max = max;
				values.flags = flags;
				register_overrides[game::dvar_type::vec3][name] = std::move(values);
			}

			void set_bool(const std::string& name, const bool value)
			{
				set_overrides[game::dvar_type::boolean][name].value.enabled = value;
			}

			void set_float(const std::string& name, const float value)
			{
				set_overrides[game::dvar_type::value][name].value.value = value;
			}

			void set_int(const std::string& name, const int value)
			{
				set_overrides[game::dvar_type::integer][name].value.integer = value;
			}

			void set_string(const std::string& name, const std::string& value)
			{
				set_overrides[game::dvar_type::string][name].string = value;
			}
		}

		utils::hook::detour dvar_register_new_hook;
		utils::hook::detour dvar_re_register_hook;

		utils::hook::detour dvar_set_variant_hook;

		void dvar_register_bool(const int hash, const char* name, bool& value, unsigned int& flags)
		{
			auto* var = find_dvar(override::register_overrides[game::dvar_type::boolean], hash);
			if (var)
			{
				value = var->value.enabled;
				flags = var->flags;
			}
		}

		void dvar_register_float(const int hash, const char* name, float& value, float& min, float& max, unsigned int& flags)
		{
			auto* var = find_dvar(override::register_overrides[game::dvar_type::value], hash);
			if (var)
			{
				value = var->value.value;
				min = var->limits.value.min;
				max = var->limits.value.max;
				flags = var->flags;
			}
		}

		void dvar_register_int(const int hash, const char* name, int& value, int& min, int& max, unsigned int& flags)
		{
			auto* var = find_dvar(override::register_overrides[game::dvar_type::integer], hash);
			if (var)
			{
				value = var->value.integer;
				min = var->limits.integer.min;
				max = var->limits.integer.max;
				flags = var->flags;
			}
		}

		void dvar_register_string(const int hash, const char* name, const char* value, unsigned int& flags)
		{
			auto* var = find_dvar(override::register_overrides[game::dvar_type::string], hash);
			if (var)
			{
				value = var->string.data();
				flags = var->flags;
			}
		}

		void dvar_register_vector2(const int hash, const char* name, float& x, float& y, float& min, float& max,
			unsigned int flags)
		{
			auto* var = find_dvar(override::register_overrides[game::dvar_type::vec2], hash);
			if (var)
			{
				x = var->value.vector[0];
				y = var->value.vector[1];
				min = var->limits.vector.min;
				max = var->limits.vector.max;
				flags = var->flags;
			}
		}

		void dvar_register_vector3(const int hash, const char* name, float& x, float& y, float& z, float& min,
			float& max, unsigned int& flags)
		{
			auto* var = find_dvar(override::register_overrides[game::dvar_type::vec3], hash);
			if (var)
			{
				x = var->value.vector[0];
				y = var->value.vector[1];
				z = var->value.vector[2];
				min = var->limits.vector.min;
				max = var->limits.vector.max;
				flags = var->flags;
			}
		}

		void dvar_set_bool(game::dvar_t* dvar, bool& boolean)
		{
			auto* var = find_dvar(override::set_overrides[game::dvar_type::boolean], dvar->hash);
			if (var)
			{
				boolean = var->value.enabled;
			}
		}

		void dvar_set_float(game::dvar_t* dvar, float& fl)
		{
			auto* var = find_dvar(override::set_overrides[game::dvar_type::value], dvar->hash);
			if (var)
			{
				fl = var->value.value;
			}
		}

		void dvar_set_int(game::dvar_t* dvar, int& integer)
		{
			auto* var = find_dvar(override::set_overrides[game::dvar_type::integer], dvar->hash);
			if (var)
			{
				integer = var->value.integer;
			}
		}

		void dvar_set_string(game::dvar_t* dvar, const char* string)
		{
			auto* var = find_dvar(override::set_overrides[game::dvar_type::string], dvar->hash);
			if (var)
			{
				string = var->string.data();
			}
		}

		static void check_dvar_overrides(const int hash, const char* name, game::dvar_type type, unsigned int flags, game::dvar_value* value, game::dvar_limits* domain)
		{
			switch (type)
			{
			case game::dvar_type::boolean:
				dvar_register_bool(hash, name, value->enabled, flags);
				break;
			case game::dvar_type::value:
				dvar_register_float(hash, name, value->value, domain->value.min, domain->value.max, flags);
				break;
			case game::dvar_type::integer:
				dvar_register_int(hash, name, value->integer, domain->integer.min, domain->integer.max, flags);
				break;
			case game::dvar_type::string:
				dvar_register_string(hash, name, value->string, flags);
				break;
			case game::dvar_type::vec2:
				dvar_register_vector2(hash, name, value->vector[0], value->vector[1], domain->vector.min, domain->vector.max, flags);
				break;
			case game::dvar_type::vec3:
				dvar_register_vector3(hash, name, value->vector[0], value->vector[1], value->vector[2], domain->vector.min, domain->vector.max, flags);
				break;
			default:
				break;
			}
		}

		game::dvar_t* dvar_register_new_stub(const int hash, const char* name, game::dvar_type type, unsigned int flags, game::dvar_value* value, game::dvar_limits* domain)
		{
			check_dvar_overrides(hash, name, type, flags, value, domain);

			return dvar_register_new_hook.invoke<game::dvar_t*>(hash, name, type, flags, value, domain);
		}

		void dvar_re_register_stub(game::dvar_t* dvar, const int hash, const char* name, game::dvar_type type, unsigned int flags, game::dvar_value* resetValue, game::dvar_limits* domain)
		{
			check_dvar_overrides(dvar->hash, name, type, flags, resetValue, domain);

			return dvar_re_register_hook.invoke<void>(dvar, hash, name, type, flags, resetValue, domain);
		}

		void dvar_set_variant_stub(game::dvar_t* dvar, game::dvar_value* value, game::DvarSetSource source)
		{
			switch (dvar->type)
			{
			case game::dvar_type::boolean:
				dvar_set_bool(dvar, value->enabled);
				break;
			case game::dvar_type::value:
				dvar_set_float(dvar, value->value);
				break;
			case game::dvar_type::integer:
				dvar_set_int(dvar, value->integer);
				break;
			case game::dvar_type::string:
				dvar_set_string(dvar, value->string);
				break;
			default:
				break;
			}

			return dvar_set_variant_hook.invoke<void>(dvar, value, source);
		}

		class component final : public component_interface
		{
		public:
			void post_load() override
			{
				dvar_register_new_hook.create(0x1404FC730, dvar_register_new_stub);
				dvar_re_register_hook.create(0x1404FC9F0, dvar_re_register_stub);
				dvar_set_variant_hook.create(0x1404FD970, dvar_set_variant_stub);
			}
		};
	}
}

REGISTER_COMPONENT_H1(h1::dvars::component)