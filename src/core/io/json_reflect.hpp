#pragma once

#include "zone/zone_memory.hpp"

#pragma warning(push, 0)
#include <glaze/core/reflect.hpp>
#include <glaze/core/tuple.hpp>
#pragma warning(pop)

namespace zonetool::json_reflect
{
	enum class null_string
	{
		empty,
		null,
	};

	struct options
	{
		null_string strings = null_string::empty;
		null_string assets = null_string::empty;
	};

	template <typename member_type>
	struct renamed_field
	{
		std::string_view key;
		member_type member;
	};

	template <typename member_type, typename fields_type>
	struct nested_field
	{
		static constexpr bool has_fields = !std::is_same_v<fields_type, std::nullptr_t>;

		std::string_view key;
		member_type member;
		const fields_type* fields;
	};

	template <typename member_type, typename value_type, typename json_type>
	struct converted_field
	{
		std::string_view key;
		member_type member;
		json_type (*to_json)(value_type);
		value_type (*from_json)(json_type);
	};

	template <typename member_type, typename count_type, typename fields_type>
	struct counted_field
	{
		static constexpr bool has_fields = !std::is_same_v<fields_type, std::nullptr_t>;
		static constexpr bool fixed_count = std::is_integral_v<count_type>;

		std::string_view key;
		member_type member;
		count_type count;
		const fields_type* fields;
	};

	template <typename fields_type>
	struct group_field
	{
		std::string_view key;
		const fields_type* fields;
	};

	struct manual_field
	{
		std::string_view key;
	};

	template <typename member_type>
	constexpr renamed_field<member_type> renamed(const std::string_view key, const member_type member)
	{
		return {key, member};
	}

	template <typename member_type>
	constexpr nested_field<member_type, std::nullptr_t> nested(const member_type member)
	{
		return {{}, member, nullptr};
	}

	template <typename member_type, typename fields_type>
	constexpr nested_field<member_type, fields_type> nested(const member_type member, const fields_type& fields)
	{
		return {{}, member, &fields};
	}

	template <typename member_type, typename fields_type>
	constexpr nested_field<member_type, fields_type> nested(const std::string_view key, const member_type member, const fields_type& fields)
	{
		return {key, member, &fields};
	}

	template <typename member_type, typename value_type, typename json_type>
	constexpr converted_field<member_type, value_type, json_type> converted(const member_type member,
		json_type (*to_json)(value_type), value_type (*from_json)(json_type))
	{
		return {{}, member, to_json, from_json};
	}

	template <typename member_type, typename value_type, typename json_type>
	constexpr converted_field<member_type, value_type, json_type> converted(const std::string_view key, const member_type member,
		json_type (*to_json)(value_type), value_type (*from_json)(json_type))
	{
		return {key, member, to_json, from_json};
	}

	template <typename member_type, typename count_type>
	constexpr counted_field<member_type, count_type, std::nullptr_t> counted(const member_type member, const count_type count)
	{
		return {{}, member, count, nullptr};
	}

	template <typename member_type, typename count_type, typename fields_type>
	constexpr counted_field<member_type, count_type, fields_type> counted(const member_type member, const count_type count, const fields_type& fields)
	{
		return {{}, member, count, &fields};
	}

	template <typename fields_type>
	constexpr group_field<fields_type> group(const std::string_view key, const fields_type& fields)
	{
		return {key, &fields};
	}

	constexpr manual_field manual(const std::string_view key)
	{
		return {key};
	}

	template <typename T>
	concept string_pointer = std::is_same_v<T, const char*> || std::is_same_v<T, char*>;

	template <typename T>
	concept asset_reference = std::is_pointer_v<T> && std::is_class_v<std::remove_pointer_t<T>> && requires(T asset)
	{
		{ asset->name } -> std::convertible_to<const char*>;
	};

	template <typename T>
	concept auto_reflected = std::is_class_v<T> && glz::reflectable<T> && glz::reflect<T>::keys.size() > 0;

	template <typename T>
	concept nested_entry = requires(T field) { field.member; field.fields; };

	template <typename T>
	concept group_entry = !nested_entry<T> && requires(T field) { field.fields; };

	template <typename T>
	concept converted_entry = requires(T field) { field.to_json; field.from_json; };

	template <typename T>
	concept counted_entry = requires(T field) { field.count; };

	template <typename T>
	void write(auto& data, const T& value, const options& opts = {});

	template <typename T>
	void read(const auto& data, T& value, zone_memory* mem, const options& opts = {});

	template <const auto& fields, typename T>
	void write(auto& data, const T& value, const options& opts = {});

	template <const auto& fields, typename T>
	void read(const auto& data, T& value, zone_memory* mem, const options& opts = {});

	template <typename T>
	void write_array(auto& data, const T* values, std::size_t count, const options& opts = {});

	template <const auto& fields, typename T>
	void write_array(auto& data, const T* values, std::size_t count, const options& opts = {});

	template <typename T, typename count_type>
	void read_array(const auto& data, T*& values, count_type& count, zone_memory* mem, const options& opts = {});

	template <const auto& fields, typename T, typename count_type>
	void read_array(const auto& data, T*& values, count_type& count, zone_memory* mem, const options& opts = {});

	namespace detail
	{
		template <const auto& fields, std::size_t index>
		constexpr auto field_member()
		{
			constexpr auto& field = glz::get<index>(fields);
			if constexpr (std::is_member_object_pointer_v<std::remove_cvref_t<decltype(field)>>)
			{
				return field;
			}
			else
			{
				return field.member;
			}
		}

		template <typename T>
		constexpr bool has_key([[maybe_unused]] const T& field)
		{
			if constexpr (std::is_member_object_pointer_v<T>)
			{
				return false;
			}
			else
			{
				return !field.key.empty();
			}
		}

		template <const auto& fields, std::size_t index>
		constexpr std::string_view field_key()
		{
			constexpr auto& field = glz::get<index>(fields);
			if constexpr (has_key(field))
			{
				return field.key;
			}
			else
			{
				return glz::get_name<field_member<fields, index>()>();
			}
		}

		template <const auto& fields>
		constexpr auto field_indices()
		{
			return std::make_index_sequence<glz::tuple_size_v<std::remove_cvref_t<decltype(fields)>>>{};
		}

		template <typename T>
		void write_members(auto& data, const T& value, const options& opts)
		{
			constexpr auto keys = glz::reflect<T>::keys;

			[&]<std::size_t... index>(std::index_sequence<index...>)
			{
				(write(data[std::string(keys[index])], glz::get_member(value, glz::get<index>(glz::to_tie(value))), opts), ...);
			}(std::make_index_sequence<keys.size()>{});
		}

		template <typename T>
		void read_members(const auto& data, T& value, zone_memory* mem, const options& opts)
		{
			constexpr auto keys = glz::reflect<T>::keys;

			const auto read_member = [&](const std::string_view key, auto& member)
			{
				const auto entry = data.find(key);
				if (entry != data.end() && !entry->is_null())
				{
					read(*entry, member, mem, opts);
				}
			};

			[&]<std::size_t... index>(std::index_sequence<index...>)
			{
				(read_member(keys[index], glz::get_member(value, glz::get<index>(glz::to_tie(value)))), ...);
			}(std::make_index_sequence<keys.size()>{});
		}

		template <typename value_type, typename json_type>
		value_type read_converted(const auto& data, value_type (*from_json)(json_type))
		{
			if constexpr (string_pointer<json_type>)
			{
				const auto string = data.template get<std::string>();
				return from_json(string.data());
			}
			else
			{
				return from_json(data.template get<std::remove_cvref_t<json_type>>());
			}
		}

		template <typename T, typename count_type>
		void read_elements(const auto& data, T*& values, count_type& count, zone_memory* mem, const auto& read_element)
		{
			values = nullptr;
			count = 0;
			if (!data.is_array() || data.empty())
			{
				return;
			}

			auto* elements = mem->allocate<std::remove_const_t<T>>(data.size());
			for (auto i = 0u; i < data.size(); i++)
			{
				if (!data[i].is_null())
				{
					read_element(data[i], elements[i]);
				}
			}

			values = elements;
			count = static_cast<count_type>(data.size());
		}

		template <typename T>
		void read_fixed_elements(const auto& data, T*& values, const std::size_t count, zone_memory* mem, const auto& read_element)
		{
			auto* elements = mem->allocate<std::remove_const_t<T>>(count);
			const auto read_count = data.is_array() ? std::min(data.size(), count) : 0;
			for (auto i = 0u; i < read_count; i++)
			{
				if (!data[i].is_null())
				{
					read_element(data[i], elements[i]);
				}
			}

			values = elements;
		}

		template <typename field_type, typename T>
		std::size_t element_count(const field_type& field, const T& value)
		{
			if constexpr (field_type::fixed_count)
			{
				return static_cast<std::size_t>(field.count);
			}
			else
			{
				return static_cast<std::size_t>(value.*field.count);
			}
		}

		template <const auto& fields, std::size_t index>
		void write_value(auto& data, const auto& value, const options& opts)
		{
			constexpr auto& field = glz::get<index>(fields);
			if constexpr (std::remove_cvref_t<decltype(field)>::has_fields)
			{
				write<*field.fields>(data, value, opts);
			}
			else
			{
				write(data, value, opts);
			}
		}

		template <const auto& fields, std::size_t index>
		void read_value(const auto& data, auto& value, zone_memory* mem, const options& opts)
		{
			constexpr auto& field = glz::get<index>(fields);
			if constexpr (std::remove_cvref_t<decltype(field)>::has_fields)
			{
				read<*field.fields>(data, value, mem, opts);
			}
			else
			{
				read(data, value, mem, opts);
			}
		}

		template <const auto& fields, std::size_t index, typename T>
		void write_field(auto& data, const T& value, const options& opts)
		{
			constexpr auto& field = glz::get<index>(fields);
			using field_type = std::remove_cvref_t<decltype(field)>;
			auto& entry = data[std::string(field_key<fields, index>())];

			if constexpr (counted_entry<field_type>)
			{
				const auto* values = value.*field.member;
				if (!values)
				{
					return;
				}

				const auto count = element_count(field, value);
				for (auto i = 0u; i < count; i++)
				{
					write_value<fields, index>(entry[i], values[i], opts);
				}
			}
			else if constexpr (group_entry<field_type>)
			{
				write<*field.fields>(entry, value, opts);
			}
			else if constexpr (nested_entry<field_type>)
			{
				const auto& member = value.*field.member;
				if constexpr (std::is_pointer_v<std::remove_cvref_t<decltype(member)>>)
				{
					if (member)
					{
						write_value<fields, index>(entry, *member, opts);
					}
				}
				else
				{
					write_value<fields, index>(entry, member, opts);
				}
			}
			else if constexpr (std::is_same_v<field_type, manual_field>)
			{
			}
			else if constexpr (converted_entry<field_type>)
			{
				write(entry, field.to_json(value.*field.member), opts);
			}
			else
			{
				write(entry, value.*field_member<fields, index>(), opts);
			}
		}

		template <const auto& fields, std::size_t index, typename T>
		void read_field(const auto& data, T& value, zone_memory* mem, const options& opts)
		{
			constexpr auto& field = glz::get<index>(fields);
			using field_type = std::remove_cvref_t<decltype(field)>;
			const auto entry = data.find(field_key<fields, index>());
			if (entry == data.end() || entry->is_null())
			{
				return;
			}

			if constexpr (counted_entry<field_type>)
			{
				const auto read_element = [&](const auto& element_data, auto& element)
				{
					read_value<fields, index>(element_data, element, mem, opts);
				};

				if constexpr (field_type::fixed_count)
				{
					read_fixed_elements(*entry, value.*field.member, field.count, mem, read_element);
				}
				else
				{
					read_elements(*entry, value.*field.member, value.*field.count, mem, read_element);
				}
			}
			else if constexpr (group_entry<field_type>)
			{
				read<*field.fields>(*entry, value, mem, opts);
			}
			else if constexpr (nested_entry<field_type>)
			{
				auto& member = value.*field.member;
				using member_type = std::remove_cvref_t<decltype(member)>;
				if constexpr (std::is_pointer_v<member_type>)
				{
					auto* object = mem->allocate<std::remove_const_t<std::remove_pointer_t<member_type>>>();
					read_value<fields, index>(*entry, *object, mem, opts);
					member = object;
				}
				else
				{
					read_value<fields, index>(*entry, member, mem, opts);
				}
			}
			else if constexpr (std::is_same_v<field_type, manual_field>)
			{
			}
			else if constexpr (converted_entry<field_type>)
			{
				value.*field.member = read_converted(*entry, field.from_json);
			}
			else
			{
				read(*entry, value.*field_member<fields, index>(), mem, opts);
			}
		}
	}

	template <typename T>
	void write(auto& data, const T& value, const options& opts)
	{
		if constexpr (string_pointer<T>)
		{
			if (value)
			{
				data = value;
			}
			else if (opts.strings == null_string::empty)
			{
				data = "";
			}
			else
			{
				data = nullptr;
			}
		}
		else if constexpr (std::is_same_v<T, std::string>)
		{
			data = value;
		}
		else if constexpr (asset_reference<T>)
		{
			if (value)
			{
				data = value->name;
			}
			else if (opts.assets == null_string::empty)
			{
				data = "";
			}
			else
			{
				data = nullptr;
			}
		}
		else if constexpr (std::is_array_v<T> && std::is_same_v<std::remove_extent_t<T>, char>)
		{
			data = std::string(value, strnlen(value, std::extent_v<T>));
		}
		else if constexpr (std::is_array_v<T>)
		{
			for (auto i = 0u; i < std::extent_v<T>; i++)
			{
				write(data[i], value[i], opts);
			}
		}
		else if constexpr (auto_reflected<T>)
		{
			detail::write_members(data, value, opts);
		}
		else
		{
			static_assert(std::is_arithmetic_v<T> || std::is_enum_v<T>, "type has no JSON mapping, use a field list or handle it by hand");
			data = value;
		}
	}

	template <typename T>
	void read(const auto& data, T& value, zone_memory* mem, const options& opts)
	{
		if constexpr (string_pointer<T>)
		{
			const auto string = data.template get<std::string>();
			if (string.empty() && opts.strings == null_string::empty)
			{
				value = nullptr;
			}
			else
			{
				value = mem->duplicate_string(string);
			}
		}
		else if constexpr (asset_reference<T>)
		{
			const auto name = data.template get<std::string>();
			if (name.empty())
			{
				value = nullptr;
			}
			else
			{
				auto* asset = mem->allocate<std::remove_cvref_t<std::remove_pointer_t<T>>>();
				asset->name = mem->duplicate_string(name);
				value = asset;
			}
		}
		else if constexpr (std::is_array_v<T> && std::is_same_v<std::remove_extent_t<T>, char>)
		{
			const auto string = data.template get<std::string>();
			const auto length = std::min(string.size(), std::extent_v<T> - 1);
			std::memcpy(value, string.data(), length);
			value[length] = 0;
		}
		else if constexpr (std::is_array_v<T>)
		{
			const auto count = std::min(data.size(), std::extent_v<T>);
			for (auto i = 0u; i < count; i++)
			{
				if (!data[i].is_null())
				{
					read(data[i], value[i], mem, opts);
				}
			}
		}
		else if constexpr (auto_reflected<T>)
		{
			detail::read_members(data, value, mem, opts);
		}
		else
		{
			static_assert(std::is_arithmetic_v<T> || std::is_enum_v<T>, "type has no JSON mapping, use a field list or handle it by hand");
			data.get_to(value);
		}
	}

	template <const auto& fields, typename T>
	void write(auto& data, const T& value, const options& opts)
	{
		[&]<std::size_t... index>(std::index_sequence<index...>)
		{
			(detail::write_field<fields, index>(data, value, opts), ...);
		}(detail::field_indices<fields>());
	}

	template <const auto& fields, typename T>
	void read(const auto& data, T& value, zone_memory* mem, const options& opts)
	{
		[&]<std::size_t... index>(std::index_sequence<index...>)
		{
			(detail::read_field<fields, index>(data, value, mem, opts), ...);
		}(detail::field_indices<fields>());
	}

	template <typename T>
	void write_array(auto& data, const T* values, const std::size_t count, const options& opts)
	{
		for (auto i = 0u; i < count; i++)
		{
			write(data[i], values[i], opts);
		}
	}

	template <const auto& fields, typename T>
	void write_array(auto& data, const T* values, const std::size_t count, const options& opts)
	{
		for (auto i = 0u; i < count; i++)
		{
			write<fields>(data[i], values[i], opts);
		}
	}

	template <typename T, typename count_type>
	void read_array(const auto& data, T*& values, count_type& count, zone_memory* mem, const options& opts)
	{
		detail::read_elements(data, values, count, mem, [&](const auto& entry, auto& value)
		{
			read(entry, value, mem, opts);
		});
	}

	template <const auto& fields, typename T, typename count_type>
	void read_array(const auto& data, T*& values, count_type& count, zone_memory* mem, const options& opts)
	{
		detail::read_elements(data, values, count, mem, [&](const auto& entry, auto& value)
		{
			read<fields>(entry, value, mem, opts);
		});
	}
}
