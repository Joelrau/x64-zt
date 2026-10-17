#pragma once

namespace zonetool::s2
{
	class json_field_dumper
	{
	public:
		explicit json_field_dumper(ordered_json& data)
			: data_(data)
		{
		}

		template <typename T>
		void value(const char* name, const T& field) const
		{
			this->data_[name] = field;
		}

		void string(const char* name, const char* field) const
		{
			this->data_[name] = string_value(field);
		}

		template <std::size_t N>
		void strings(const char* name, const char* (&field)[N]) const
		{
			auto& out = this->data_[name] = ordered_json::array();
			for (const auto* str : field)
			{
				out.push_back(string_value(str));
			}
		}

		template <typename T>
		void asset(const char* name, T* field) const
		{
			this->data_[name] = asset_value(field);
		}

		template <typename T, std::size_t N>
		void assets(const char* name, T* (&field)[N]) const
		{
			this->data_[name] = asset_values(field, N);
		}

		template <typename T>
		void asset_array(const char* name, T** field, const std::size_t count) const
		{
			this->data_[name] = field ? asset_values(field, count) : nullptr;
		}

		template <typename T, typename C>
		void asset_list(const char* name, T** field, const C count) const
		{
			this->asset_array(name, field, count);
		}

		template <typename T, std::size_t N>
		void named_assets(const char* name, T** field, const char* const (&names)[N]) const
		{
			if (!field)
			{
				this->data_[name] = nullptr;
				return;
			}

			auto& out = this->data_[name] = ordered_json::object();
			for (auto i = 0u; i < N; i++)
			{
				if (field[i])
				{
					out[names[i]] = asset_name(field[i]);
				}
			}
		}

		void script_string(const char* name, const scr_string_t field) const
		{
			this->data_[name] = string_value(SL_ConvertToString(field));
		}

		void script_strings(const char* name, const scr_string_t* field, const std::size_t count) const
		{
			if (!field)
			{
				this->data_[name] = nullptr;
				return;
			}

			auto& out = this->data_[name] = ordered_json::array();
			for (auto i = 0u; i < count; i++)
			{
				out.push_back(string_value(SL_ConvertToString(field[i])));
			}
		}

		template <typename T>
		void array(const char* name, const T* field, const std::size_t count) const
		{
			this->data_[name] = field ? ordered_json(std::vector<T>(field, field + count)) : nullptr;
		}

		template <typename T, typename C>
		void array_list(const char* name, const T* field, const C count) const
		{
			this->array(name, field, count);
		}

		template <std::size_t N>
		void graph_knots(const char* knots_name, vec2_t* (&knots)[N], const char* original_name, vec2_t* (&original)[N],
			const std::uint16_t (&counts)[N]) const
		{
			auto& knots_out = this->data_[knots_name] = ordered_json::array();
			auto& original_out = this->data_[original_name] = ordered_json::array();
			for (auto i = 0u; i < N; i++)
			{
				knots_out.push_back(knots_value(knots[i], counts[i]));
				original_out.push_back(knots_value(original[i], counts[i]));
			}
		}

		template <typename T, typename C, typename F>
		void entries(const char* name, T* field, const C count, F&& callback) const
		{
			if (!field)
			{
				this->data_[name] = nullptr;
				return;
			}

			auto& out = this->data_[name] = ordered_json::array();
			for (auto i = 0u; i < count; i++)
			{
				auto entry = ordered_json::object();
				callback(json_field_dumper(entry), field[i]);
				out.push_back(std::move(entry));
			}
		}

		template <typename T, typename F>
		void object(const char* name, T& field, F&& callback) const
		{
			auto& out = this->data_[name] = ordered_json::object();
			callback(json_field_dumper(out), field);
		}

		template <typename T, typename F>
		void optional_object(const char* name, T* field, F&& callback) const
		{
			if (!field)
			{
				this->data_[name] = nullptr;
				return;
			}

			this->object(name, *field, callback);
		}

	private:
		static ordered_json string_value(const char* str)
		{
			return str ? ordered_json(str) : ordered_json(nullptr);
		}

		template <typename T>
		static ordered_json asset_value(T* asset)
		{
			return asset ? asset_name(asset) : "";
		}

		template <typename T>
		static ordered_json asset_values(T** assets, const std::size_t count)
		{
			auto out = ordered_json::array();
			for (auto i = 0u; i < count; i++)
			{
				out.push_back(asset_value(assets[i]));
			}

			return out;
		}

		static ordered_json knots_value(const vec2_t* knots, const std::size_t count)
		{
			if (!knots)
			{
				return nullptr;
			}

			auto out = ordered_json::array();
			for (auto i = 0u; i < count; i++)
			{
				out.push_back({knots[i][0], knots[i][1]});
			}

			return out;
		}

		ordered_json& data_;
	};

	class json_field_parser
	{
	public:
		json_field_parser(const json& data, zone_memory* mem)
			: data_(data)
			, mem_(mem)
		{
		}

		template <typename T>
		void value(const char* name, T& field) const
		{
			if (const auto* node = this->find(name))
			{
				read_value(*node, field);
			}
		}

		void string(const char* name, const char*& field) const
		{
			field = this->parse_string(this->find(name));
		}

		template <std::size_t N>
		void strings(const char* name, const char* (&field)[N]) const
		{
			const auto* node = this->find(name);
			for (auto i = 0u; i < N; i++)
			{
				field[i] = this->parse_string(element(node, i));
			}
		}

		template <typename T>
		void asset(const char* name, T*& field) const
		{
			field = this->parse_asset<T>(this->find(name));
		}

		template <typename T, std::size_t N>
		void assets(const char* name, T* (&field)[N]) const
		{
			const auto* node = this->find(name);
			for (auto i = 0u; i < N; i++)
			{
				field[i] = this->parse_asset<T>(element(node, i));
			}
		}

		template <typename T>
		void asset_array(const char* name, T**& field, const std::size_t count) const
		{
			const auto* node = this->find(name);
			field = node ? this->allocate<T*>(count) : nullptr;
			for (auto i = 0u; field && i < count; i++)
			{
				field[i] = this->parse_asset<T>(element(node, i));
			}
		}

		template <typename T, typename C>
		void asset_list(const char* name, T**& field, C& count) const
		{
			const auto* node = this->find(name);
			count = static_cast<C>(node ? node->size() : 0);
			this->asset_array(name, field, count);
		}

		template <typename T, std::size_t N>
		void named_assets(const char* name, T**& field, const char* const (&names)[N]) const
		{
			const auto* node = this->find(name);
			field = node ? this->allocate<T*>(N) : nullptr;
			if (!field)
			{
				return;
			}

			for (const auto& [key, value] : node->items())
			{
				const auto index = name_index(names, key);
				if (index >= N)
				{
					ZONETOOL_FATAL("Unknown name \"%s\" in \"%s\"", key.data(), name);
				}

				field[index] = this->parse_asset<T>(&value);
			}
		}

		void script_string(const char* name, scr_string_t& field) const
		{
			field = parse_script_string(this->find(name));
		}

		void script_strings(const char* name, scr_string_t*& field, const std::size_t count) const
		{
			const auto* node = this->find(name);
			field = node ? this->allocate<scr_string_t>(count) : nullptr;
			for (auto i = 0u; field && i < count; i++)
			{
				field[i] = parse_script_string(element(node, i));
			}
		}

		template <typename T>
		void array(const char* name, T*& field, const std::size_t count) const
		{
			const auto* node = this->find(name);
			field = node ? this->allocate<T>(count) : nullptr;
			for (auto i = 0u; field && i < count && i < node->size(); i++)
			{
				read_value((*node)[i], field[i]);
			}
		}

		template <typename T, typename C>
		void array_list(const char* name, T*& field, C& count) const
		{
			const auto* node = this->find(name);
			count = static_cast<C>(node ? node->size() : 0);
			this->array(name, field, count);
		}

		template <std::size_t N>
		void graph_knots(const char* knots_name, vec2_t* (&knots)[N], const char* original_name, vec2_t* (&original)[N],
			std::uint16_t (&counts)[N]) const
		{
			const auto* knots_node = this->find(knots_name);
			const auto* original_node = this->find(original_name);
			for (auto i = 0u; i < N; i++)
			{
				const auto* graph = element(knots_node, i);
				const auto* original_graph = element(original_node, i);
				const auto count = graph ? graph->size() : 0;
				if (original_graph && original_graph->size() != count)
				{
					ZONETOOL_FATAL("Weapon \"%s\" and \"%s\" %u have a different knot count", knots_name, original_name, i);
				}

				counts[i] = static_cast<std::uint16_t>(count);
				knots[i] = this->parse_knots(graph, count);
				original[i] = this->parse_knots(original_graph, count);
			}
		}

		template <typename T, typename C, typename F>
		void entries(const char* name, T*& field, C& count, F&& callback) const
		{
			const auto* node = this->find(name);
			count = static_cast<C>(node ? node->size() : 0);
			field = node ? this->allocate<T>(count) : nullptr;
			for (auto i = 0u; field && i < count; i++)
			{
				callback(json_field_parser((*node)[i], this->mem_), field[i]);
			}
		}

		template <typename T, typename F>
		void object(const char* name, T& field, F&& callback) const
		{
			if (const auto* node = this->find(name))
			{
				callback(json_field_parser(*node, this->mem_), field);
			}
		}

		template <typename T, typename F>
		void optional_object(const char* name, T*& field, F&& callback) const
		{
			const auto* node = this->find(name);
			field = node ? this->allocate<T>(1) : nullptr;
			if (field)
			{
				callback(json_field_parser(*node, this->mem_), *field);
			}
		}

	private:
		const json* find(const char* name) const
		{
			const auto node = this->data_.find(name);
			return node != this->data_.end() && !node->is_null() ? &*node : nullptr;
		}

		static const json* element(const json* node, const std::size_t index)
		{
			return node && node->is_array() && index < node->size() && !(*node)[index].is_null() ? &(*node)[index] : nullptr;
		}

		template <typename T>
		static void read_value(const json& node, T& field)
		{
			if constexpr (std::is_array_v<T>)
			{
				for (auto i = 0u; i < std::extent_v<T> && i < node.size(); i++)
				{
					read_value(node[i], field[i]);
				}
			}
			else
			{
				node.get_to(field);
			}
		}

		template <std::size_t N>
		static std::size_t name_index(const char* const (&names)[N], const std::string& key)
		{
			for (auto i = 0u; i < N; i++)
			{
				if (!_stricmp(names[i], key.data()))
				{
					return i;
				}
			}

			return N;
		}

		static scr_string_t parse_script_string(const json* node)
		{
			return node ? static_cast<scr_string_t>(SL_GetString(node->get_ref<const std::string&>().data())) : 0;
		}

		// a zero count array keeps one element, so that the writer keeps the non-null pointer of stock zones
		template <typename T>
		T* allocate(const std::size_t count) const
		{
			return this->mem_->allocate<T>(std::max<std::size_t>(count, 1));
		}

		const char* parse_string(const json* node) const
		{
			return node ? this->mem_->duplicate_string(node->get_ref<const std::string&>()) : nullptr;
		}

		template <typename T>
		T* parse_asset(const json* node) const
		{
			if (!node || node->get_ref<const std::string&>().empty())
			{
				return nullptr;
			}

			auto* stub = this->allocate<T>(1);
			asset_name(stub) = this->mem_->duplicate_string(node->get_ref<const std::string&>());
			return stub;
		}

		vec2_t* parse_knots(const json* node, const std::size_t count) const
		{
			auto* knots = node ? this->allocate<vec2_t>(count) : nullptr;
			for (auto i = 0u; knots && i < count; i++)
			{
				read_value((*node)[i], knots[i]);
			}

			return knots;
		}

		const json& data_;
		zone_memory* mem_;
	};
}
